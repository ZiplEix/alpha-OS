#include "alphaos.h"

int main(int argc, char const *argv[])
{
    print("Hello, World!\n");

    while (1) {
        if (getkey() != 0) {
            print("You pressed a key!\n");
        }
    }

    return 0;
}
