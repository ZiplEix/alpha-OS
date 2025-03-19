#include "alphaos.h"
#include "stdlib.h"

int main(int argc, char const *argv[])
{
    print("Hello, World!\n");

    print(itoa(45678));

    void *ptr = malloc(512);
    free(ptr);

    while (1) {
        if (getkey() != 0) {
            print("You pressed a key!\n");
        }
    }

    return 0;
}
