#ifndef ALPHAOS_H
#define ALPHAOS_H

#include <stddef.h>
#include <stdbool.h>

void print(const char *filename);
int __getkey();
int __getkeyblock();
void __putchar(char c);

void *__malloc(size_t size);
void __free(void *ptr);

void terminal_readline(char *out, int max, bool output_while_typing);

#endif // ALPHAOS_H
