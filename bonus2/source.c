#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int language = 0;

extern char DAT_0804873d; // "fi"
extern char DAT_08048740; // "nl"


char global_buffer[76];
void greetuser(void) {
    char greeting[64]; 
    
    if (language == 1) // Finnish 
        strcpy(greeting, "Hyvää päivää ");
    else if (language == 2) // Dutch
        strcpy(greeting, "Goedemiddag! ");
    else // English
        strcpy(greeting, "Hello ");
    strcat(greeting, global_buffer);
    puts(greeting);
}


int main(int argc, char **argv) {
    char local_60[40];
    char acStack_38[36];
    char dest_buffer[76];
    char *lang_env;
    
    if (argc == 3) {
        memset(local_60, 0, sizeof(local_60));
        
        strncpy(local_60, argv[1], 0x28);
        
        strncpy(acStack_38, argv[2], 0x20);
        
        lang_env = getenv("LANG");
        if (lang_env != NULL) {
            if (memcmp(lang_env, &DAT_0804873d, 2) == 0) {
                language = 1;
            }
            else if (memcmp(lang_env, &DAT_08048740, 2) == 0) {
                language = 2;
            }
        }
        memcpy(global_buffer, local_60, 76);
        greetuser();
    } else {
        return 1;
    }
    return 0;
}