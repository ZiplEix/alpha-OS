#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "task/process.h"

typedef int (*KEYBOARD_INIT_FUNCTION)(void);

struct keyboard {
    KEYBOARD_INIT_FUNCTION init;
    char name[20];
    struct keyboard *next;
};

void keyboard_init(void);
int keyboard_insert(struct keyboard *keyboard);
void keyboard_backspace(struct process *process);
void keyboard_push(char c);
char keyboard_pop();

#endif // KEYBOARD_H
