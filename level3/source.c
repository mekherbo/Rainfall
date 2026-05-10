#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t m=0;
void v(void)
{
  char buffer [520];
  
  fgets(buffer,0x200,stdin);
  printf(buffer);  // this is a format string vulnerability, we can use it to write to the variable m
										/*level3@RainFall:~$ objdump -t ./level3 | grep 0804988c
											0804988c g     O .bss	00000004              m
											this variable is 4 bytes long so we can declare it as a uint32_t
										*/
  if (m == 0x40) {
    fwrite("Wait what?!\n",1,12,stdout);
    system("/bin/sh");
  }
  return;
}

int main(void)
{
	v();
	return 0;
}