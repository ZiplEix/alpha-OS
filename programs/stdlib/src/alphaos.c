#include <stddef.h>
#include <stdbool.h>

#include "alphaos.h"

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
