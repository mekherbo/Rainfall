# bonus 3 

## Ghidra Analysis

``` 

    iVar2 = atoi(*(char **)(param_2 + 4));
    local_98[iVar2] = '\0';
    fread(local_56,1,0x41,local_14);
    fclose(local_14);
    iVar2 = strcmp(local_98,*(char **)(param_2 + 4));
    if (iVar2 == 0) {
      execl("/bin/sh","sh",0);
    }
    else {
      puts(local_56);
    }
```
By passing an argument that evaluates to 0 via atoi() (such as an empty string "" or "0"), we trigger a null-byte insertion at buffer[0]. This immediately truncates the password buffer. Since the truncated buffer is now an empty string, it successfully matches our empty string argument in the strcmp check, validating the condition and spawning a shell.

## payload

./bonus3 ""


## flag

3321b6f81659f9a71c76616f606e4b50189cecfea611393d5d649f75e157353c
