#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>
int main(void)
{
	int32_t n = INT32_MIN;
	while (n * 4 < 44)
		n++;
	printf("%d\n", n);
	return 0;
}