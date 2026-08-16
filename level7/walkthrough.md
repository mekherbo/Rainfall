# level7

## Binary Analysis and Memory Block Representation (Heap Layout)

When `main` executes, it performs a series of `malloc(8)` allocations. On the heap, the blocks are laid out sequentially:

The program allocates two main structures on the heap:

* **`block1`**: Contains an integer value (`1`) and a pointer to an 8-byte buffer allocated via `malloc`.
* **`block2`**: Contains an integer value (`2`) and a pointer to an 8-byte buffer allocated via `malloc`.

Visually, they are laid out sequentially on the heap:

```text
[ block1 ] -> [ Value: 1 ] [ Ptr to Buffer 1 (8 bytes) ]
[ block2 ] -> [ Value: 2 ] [ Ptr to Buffer 2 (8 bytes) ]
```

## Vulnerability

strcpy copies argv[1] into Buffer 1 and argv[2] into Buffer 2 without any bounds checking.
Because block1's buffer is adjacent to block2 in memory, overflowing argv[1] allows you to overwrite the contents of block2 (including its pointer or metadata).
Function m() reads and prints the password from /home/user/level8/.pass, but it isn't called during normal execution.
By leveraging the heap overflow from block1 into block2, we can manipulate pointers or override target execution paths to force the program to jump to m() (0x080484f4) and reveal the password.

### GOT Overwrite & Trigger

The Final Trigger (puts): At the end of main, the program calls puts("~~"). Normally, this looks up the puts function address in the Global Offset Table (GOT) and executes it.

Targeting the GOT via Heap Overwrite: Because the heap layout places block1 and block2 contiguously, overflowing argv[1] allows us to overwrite pointers within block2. By precisely crafting our input, we can overwrite the function pointer/destination structure or target memory pointers so that the program writes the address of function m (0x080484f4) into the GOT entry for puts.

Execution: When the program reaches the final puts() call, it fetches the address from the overwritten GOT entry, jumps to m() instead, and prints the password/flag.

## Payload

### Table correspondance

| Symbol / Target | Memory Address | Little-Endian Hex Representation |
| :--- | :--- | :--- |
| **`puts` (GOT Entry)** | `0x08049928` | `\x28\x99\x04\x08` |
| **`m` (Function Address)** | `0x080484f4` | `\xf4\x84\x04\x08` |

$(python -c 'print "A"*20 + "\x28\x99\x04\x08"') $'\xf4\x84\x04\x08'

## flag
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
