#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
    int input_val;
    char *shell_path;
    uid_t euid;
    gid_t egid;

    input_val = atoi(argv[1]);

    // Check if the input matches the magic number 0x1a7 (423 decimal)
    if (input_val == 423) {
        shell_path = strdup("/bin/sh");
        
        // Get the Effective User ID and Group ID (which are 'level1' due to SetUID)
        egid = getegid();
        euid = geteuid();
        
        // Apply the privileges to the current process
        setresgid(egid, egid, egid);
        setresuid(euid, euid, euid);
        
        execv("/bin/sh", (char *[]){shell_path, NULL});
    }
    else
        fwrite("No !\n", 1, 5, stderr);
    return 0;
}