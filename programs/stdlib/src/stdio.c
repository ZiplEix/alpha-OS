
#include <stdarg.h>

#include "stdio.h"
#include "alphaos.h"
#include "stdlib.h"

int putchar(int c)
{
    __putchar((char)c);
    return 0;
}

int printf(const char *format, ...)
{
    va_list ap;
    const char *p;
    char *sval;
    int ival;

    va_start(ap, format);
    for (p = format; *p; p++) {
        if (*p != '%') {
            putchar(*p);
            continue;
        }

        switch (*++p) {
            case 'i':
            case 'd':
                ival = va_arg(ap, int);
                print(itoa(ival));
                break;
            case 's':
                sval = va_arg(ap, char *);
                print(sval);
                break;
            case 'c':
                ival = va_arg(ap, int);
                putchar(ival);
                break;
            case '%':
                putchar('%');
                break;
            default:
                putchar(*p);
                break;
        }
    }

    va_end(ap);

    return 0;
}
