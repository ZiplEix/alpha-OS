#include "stdlib.h"
#include "alphaos.h"

// This function is used to convert an integer to a string.
// The value return by the function is a static pointer to a string.
// The value returned by the function is overwritten by the next call to the function,
// so it need to be copied to another string if it is needed to be used later.
char *itoa(int value)
{
    static char text[12];
    int loc = 11;
    text[11] = 0;
    char neg = 1;

    if (value >= 0) {
        neg = 0;
        value = -value;
    }

    while(value) {
        text[--loc] = '0' - (value % 10);
        value /= 10;
    }

    if (loc == 11) {
        text[--loc] = '0';
    }

    if (neg) {
        text[--loc] = '-';
    }

    return &text[loc];
}

void *malloc(size_t size)
{
    return __malloc(size);
}

void free(void *ptr)
{
    __free(ptr);
}
