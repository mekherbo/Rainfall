#include <stdio.h>
#include <stdlib.h>

void run(void)
{
  fwrite("Good... Wait what?\n",1,0x13,stdout);
  system("/bin/sh");
}

int main(void)
{
	char *input[76];

	gets(input);
	return 0;
}