#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "task/process.h"

#define KEYBOARD_CAPS_LOCK_ON 1
#define KEYBOARD_CAPS_LOCK_OFF 0
typedef int KEYBAORD_CAPS_LOCK_STATE;

typedef int (*KEYBOARD_INIT_FUNCTION)(void);

struct keyboard {
    KEYBOARD_INIT_FUNCTION init;
    char name[20];
    KEYBAORD_CAPS_LOCK_STATE capslock_state;

    struct keyboard *next;
};

void keyboard_init(void);
int keyboard_insert(struct keyboard *keyboard);
void keyboard_backspace(struct process *process);
void keyboard_push(char c);
char keyboard_pop();

void keyboard_set_capslock(struct keyboard *keyboard, KEYBAORD_CAPS_LOCK_STATE state);
KEYBAORD_CAPS_LOCK_STATE keyboard_get_capslock(struct keyboard *keyboard);

#endif // KEYBOARD_H
