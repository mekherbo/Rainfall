#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <string.h>



#define BSIZE 16

#define OVERSIZE 8 /* overflow buf2 by OVERSIZE bytes */

typedef unsigned long u_long;

void main(void) {

u_long b_diff;

char *buf0 = (char*)malloc(BSIZE); // create two buffers

char *buf1 = (char*)malloc(BSIZE);



b_diff = (u_long)buf1 - (u_long)buf0; // difference between locations

printf("Initial values: ");

printf("buf0=%p, buf1=%p, b_diff=0x%x bytes\n", buf0, buf1, b_diff);



memset(buf1, 'A', BSIZE-1), buf1[BSIZE-1] = '\0';

printf("Before overflow: buf1=%s\n", buf1);



memset(buf0, 'B', (u_int)(b_diff + OVERSIZE));

printf("After overflow: buf1=%s\n", buf1);

}