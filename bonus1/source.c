#include <stdio.h>
#include <unistd.h>

int main(int ac, char **av)
{
	char local_3c[40];
    int local_14;

    local_14 = atoi(av[1]);

    if (local_14 < 10) {
        memcpy(local_3c, av[2], local_14 * 4);

        if (local_14 == 0x574f4c46)
            execl("/bin/sh", "sh", NULL);
        return 0;
    } else {
        return 1;
    }
	return 0;
}