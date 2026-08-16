#include <stdio.h>
#include <unistd.h>
#include <limits.h>

int main(void)
{
	int n = INT_MIN;
	while (n * 4 < 44)
	{
		
		n++;
	}
	printf("%d\n", n);
	return 0;
}