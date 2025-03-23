#include "alphaos.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char const *argv[])
{
    printf("Hello, World!, a number : %d another form of integer %i\n", 98, -89);

    print("Hello, World!\n");

    print(itoa(45678));
    putchar('\n');

    putchar('Z');
    putchar('\n');

    void *ptr = malloc(512);
    free(ptr);

    char buff[1024];
    terminal_readline(buff, sizeof(buff), true);
    printf("You typed: %s\n", buff);

    while (1) {
    }

    return 0;
}
