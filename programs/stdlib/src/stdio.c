
#include <stdarg.h>

#include "stdio.h"
#include "alphaos.h"
#include "stdlib.h"

int putchar(int c)
{
    __putchar((char)c);
    return 0;
}
