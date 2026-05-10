#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t m=0;
void n(void)
{
	char buffer [520];

	fgets(buffer,0x200,stdin);
	p(buffer);
	if (m == 0x1025544) {
	system("/bin/cat /home/user/level5/.pass");
	}
	return;
}

void p(char *str)
{
  printf(str);
}

int main(void)
{
  n();
  return 0;
}