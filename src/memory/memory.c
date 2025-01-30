#include "memory.h"

void *memset(void *ptr, int value, size_t size)
{
    char *p = (char *) ptr;

    while (size--) {
        *p++ = (char)value;
    }

    return ptr;
}