# Level0

## 1. Initial Analysis
I started by identifying the file type and permissions of the binary located in the home directory.

```bash
level0@RainFall:~$ file ./level0 
./level0: setuid ELF 32-bit LSB executable, Intel 80386, version 1 (GNU/Linux), statically linked, for GNU/Linux 2.6.24, BuildID[sha1]=0x85cf4024dbe79c7ccf4f30e7c601a356ce04f412, not stripped
```
## 2. Reverse Engineering with GDB
```bash
level0@RainFall:~$ gdb ./level0
```
```nasm
(gdb) disas main
Dump of assembler code for function main:
   0x08048ec0 <+0>:	push   %ebp
   0x08048ec1 <+1>:	mov    %esp,%ebp
   0x08048ec3 <+3>:	and    $0xfffffff0,%esp
   0x08048ec6 <+6>:	sub    $0x20,%esp
   0x08048ec9 <+9>:	mov    0xc(%ebp),%eax
   0x08048ecc <+12>:	add    $0x4,%eax
   0x08048ecf <+15>:	mov    (%eax),%eax
   0x08048ed1 <+17>:	mov    %eax,(%esp)
   0x08048ed4 <+20>:	call   0x8049710 <atoi>
   0x08048ed9 <+25>:	cmp    $0x1a7,%eax
   0x08048ede <+30>:	jne    0x8048f58 <main+152>

```
the binary compare the input convert with atoi with hex value 0x1a7 which is 423 in decimal
we run binary with 423 in argument
```bash
level0@RainFall:~$ ./level0 423
$ 
```
After the shell opened, I noticed I no longer had access to the current directory (/home/user/level0) because my user identity had changed to level1.
```bash
$ ls
ls: cannot open directory .: Permission denied
$ whoami
level1
```
There is a .pass file in the level1 folder that contains the flag
```bash
$ cd /home/user/level1
$ cat .pass
```

## flag
1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a
