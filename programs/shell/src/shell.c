#include "shell.h"
#include "stdio.h"
#include "stdlib.h"
#include "alphaos.h"

int main(int argc, char const *argv[])
{
    printf("Welcome to AlphaOS v1.0.0\n");

    while (1) {
        print("> ");
        char buf[1024];
        terminal_readline(buf, 1024, true);
        putchar('\n');
        print(buf);
        putchar('\n');
    }

    return 0;
}
