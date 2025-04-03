#include "alphaos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char const *argv[])
{
    char *ptr = malloc(20);
    strcpy(ptr, "Hello, World!");

    printf("%s\n", ptr);

    free(ptr);
    printf("Freed memory\n");

    ptr[0] = 'B';

    while (1) {
    }

    return 0;
}
