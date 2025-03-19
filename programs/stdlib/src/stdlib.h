#ifndef ALPHAOS_STDLIB_H
#define ALPHAOS_STDLIB_H

#include <stddef.h>

void *malloc(size_t size);
void free(void *ptr);

char *itoa(int value);

#endif // ALPHAOS_STDLIB_H
