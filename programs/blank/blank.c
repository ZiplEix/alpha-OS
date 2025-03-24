#include "alphaos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char const *argv[])
{
    printf("Hello, World!, a number : %d another form of integer %i\n", 98, -89);

    char world[] = "hello world !";

    const char *token = strtok(world, " ");
    while (token) {
        printf("%s\n", token);
        token = strtok(0, " ");
    }

    while (1) {
    }

    return 0;
}
