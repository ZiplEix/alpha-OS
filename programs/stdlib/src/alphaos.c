#include <stddef.h>
#include <stdbool.h>

#include "alphaos.h"
#include "string.h"
#include "stdlib.h"

struct command_argument *alphaos_parse_command(const char *command, int max)
{
    struct command_argument *root_command = 0;
    char scommand[1024];
    if (max > (int)sizeof(scommand)) {
        return 0;
    }

    strncpy(scommand, command, sizeof(scommand));
    char *token = strtok(scommand, " ");
    if (!token) {
        goto out;
    }

    root_command = malloc(sizeof(struct command_argument));
    if (!root_command) {
        goto out;
    }

    strncpy(root_command->arugment, token, sizeof(root_command->arugment));
    root_command->next = 0;

    struct command_argument *current = root_command;
    token = strtok(NULL, " ");

    while (token) {
        struct command_argument *new_command = malloc(sizeof(struct command_argument));
        if (!new_command) {
            break;
        }

        strncpy(new_command->arugment, token, sizeof(new_command->arugment));
        new_command->next = 0;

        current->next = new_command;
        current = new_command;

        token = strtok(NULL, " ");
    }

out:
    return root_command;
}

int __getkeyblock() {
    int val = __getkey();

    do {
        val = __getkey();
    } while (val == 0);

    return val;
}

void terminal_readline(char *out, int max, bool output_while_typing)
{
    int i = 0;
    for (i = 0; i < max - 1; i++) {
        char c = __getkeyblock();

        if (c == 13) {
            break;
        }

        if (output_while_typing) {
            __putchar(c);
        }

        // backspace
        if (c == 0x08 && i>= 1) {
            out[i] = '\0';
            i -= 2;
            continue;
        }

        out[i] = c;
    }

    out[i] = '\0';
}
