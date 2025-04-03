#include "alphaos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char const *argv[])
{
    char str[] = "hello world";
    struct command_argument *rootcommand = alphaos_parse_command(str, sizeof(str));
    printf("rootcommand: %s\n", rootcommand->arugment);
    printf("next: %s\n", rootcommand->next->arugment);

    while (1) {
    }

    return 0;
}
