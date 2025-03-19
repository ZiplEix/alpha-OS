#ifndef ALPHAOS_H
#define ALPHAOS_H

#include <stddef.h>

void print(const char *filename);
int getkey();
void __putchar(char c);

void *__malloc(size_t size);
void __free(void *ptr);

#endif // ALPHAOS_H
