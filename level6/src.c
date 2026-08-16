#include <stdlib.h>
#include <string.h>

void m(void *param_1,int param_2,char *param_3,int param_4,int param_5)

{
  puts("Nope");
  return;
}

void n(void)

{
  system("/bin/cat /home/user/level7/.pass");
  return;
}


int main(int argc, char *argv[]) {
    char *dest;
    void (**func_ptr)(void);
    
    dest = malloc(64);
    
    func_ptr = malloc(sizeof(void *));
    
    *func_ptr = m;
    
    strcpy(dest, argv[1]);
    
    (*func_ptr)();
    
    return 0;
}