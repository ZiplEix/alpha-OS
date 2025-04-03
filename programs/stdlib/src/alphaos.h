#ifndef ALPHAOS_H
#define ALPHAOS_H

#include <stddef.h>
#include <stdbool.h>

struct command_argument {
    char arugment[512];
    struct command_argument *next;
};

struct process_arguments {
    int argc;
    char **argv;
};

void print(const char *filename);
int __getkey();
int __getkeyblock();
void __putchar(char c);

void *__malloc(size_t size);
void __free(void *ptr);

void __process_load_start(const char *filename);

void __process_get_arguments(struct process_arguments *arguments);

void terminal_readline(char *out, int max, bool output_while_typing);

struct command_argument *alphaos_parse_command(const char *command, int max);

#endif // ALPHAOS_H
