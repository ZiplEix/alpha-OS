#include "alphaos.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char const *argv[])
{
    printf("Hello, World!, a number : %d another form of integer %i\n", 98, -89);

    print("Hello, World!\n");

    print(itoa(45678));

    putchar('Z');

    void *ptr = malloc(512);
    free(ptr);

    while (1) {
        if (getkey() != 0) {
            print("You pressed a key!\n");
        }
    }

    return 0;
}
