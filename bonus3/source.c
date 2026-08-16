#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char password_buffer[65]; // local_98
    char file_content[66];   // local_56
    FILE *pass_file;
    
    pass_file = fopen("/home/user/end/.pass", "r");
    
    if (pass_file == NULL || argc != 2) {
        return -1;
    }
    
    fread(password_buffer, 1, 0x42, pass_file);
    password_buffer[64] = '\0'; 
    
    int index = atoi(argv[1]);
    
    password_buffer[index] = '\0';
    
    fread(file_content, 1, 0x41, pass_file);
    fclose(pass_file);
    
    if (strcmp(password_buffer, argv[1]) == 0) {
        execl("/bin/sh", "sh", NULL);
    } else {
        puts(file_content);
    }
    
    return 0;
}