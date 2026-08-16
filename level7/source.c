#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern char c[68]; 

void m(void *param_1, int param_2, char *param_3, int param_4, int param_5) {
    time_t current_time;
    
    current_time = time(NULL);
    printf("%s - %d\n", c, (int)current_time);
}

int main(int argc, char *argv[]) {
    void *node1_data;
    void *node2_data;
    
    int *node1 = malloc(8);
    node1[0] = 1; 
    
    node1_data = malloc(8);
    node1[1] = (int)node1_data; 
    
    int *node2 = malloc(8);
    node2[0] = 2; 
    
    node2_data = malloc(8);
    node2[1] = (int)node2_data; 
    
    if (argc > 1) {
        strcpy((char *)node1[1], argv[1]);
    }
    if (argc > 2) {
        strcpy((char *)node2[1], argv[2]);
    }
    
	FILE *pass_file = fopen("/home/user/level8/.pass", "r");
    if (pass_file != NULL) {
        fgets(c, sizeof(c), pass_file);
        fclose(pass_file);
    }
    
    puts("~~");
    
    return 0;
}