#include "memory.h"

void *memset(void *ptr, int value, size_t size)
{
    char *p = (char *) ptr;

    while (size--) {
        *p++ = (char)value;
    }

    return ptr;
}

int memcmp(const void *ptr1, const void *ptr2, size_t size)
{
    const char *p1 = (const char *) ptr1;
    const char *p2 = (const char *) ptr2;

    while (size--) {
        if (*p1 != *p2) {
            return p1[-1] < p2[-1] ? -1 : 1;
        }

        p1++;
        p2++;
    }

    return 0;
}
