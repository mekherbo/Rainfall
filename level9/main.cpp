#include <unistd.h>
#include "N.hpp"

int main(int argc, char *argv[]) {
    N *obj1;
    N *obj2;

    if (argc < 2) {
        _exit(1);
    }


	// obj 108 bytes
    obj1 = new N(5);
    
    obj2 = new N(6);

    obj1->setAnnotation(argv[1]);

    obj2->operator+(*obj1);

    return 0;
}