#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void *memset(void *ptr, int value, size_t size);
int memcmp(const void *ptr1, const void *ptr2, size_t size);

#endif // MEMORY_H
