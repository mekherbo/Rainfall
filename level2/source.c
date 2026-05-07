#include <stdio.h>
#include <stdint.h>

void p(void)
{
	void *retaddr;
	char buffer[76];

	fflush(stdout);
	gets(buffer);
	retaddr = __builtin_return_address(0);
	if (((uint32_t)retaddr & 0xb0000000) == 0xb0000000) {
        printf("(%p)\n", retaddr);
        _exit(1);
    }
	puts(buffer);
	strdup(buffer);
}

int main(void)
{
	p();
	return 0;
}