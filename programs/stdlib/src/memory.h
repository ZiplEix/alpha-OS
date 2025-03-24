#ifndef ALPHAOS_MEMORY_H
#define ALPHAOS_MEMORY_H

#include <stddef.h>

void *memset(void *ptr, int value, size_t size);
int memcmp(const void *ptr1, const void *ptr2, size_t size);
void *memcpy(void *dest, const void *src, size_t size);

#endif // ALPHAOS_MEMORY_H
