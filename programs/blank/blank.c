#include "alphaos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char const *argv[])
{
    struct process_arguments arguments;
    __process_get_arguments(&arguments);
    printf("argc: %d\n", arguments.argc);
    for (int i = 0; i < arguments.argc; i++) {
        printf("argv[%d]: %s\n", i, arguments.argv[i]);
    }
    printf("Press any key to continue...\n");

    while (1) {
    }

    return 0;
}
