#ifndef ALPHAOS_H
#define ALPHAOS_H

#include <stddef.h>

void print(const char *filename);
int getkey();

void *__malloc(size_t size);

#endif // ALPHAOS_H
