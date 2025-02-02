#include <stdint.h>
#include <stddef.h>

#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"

uint16_t *video_memory = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char color)
{
    return (color << 8) | c;
}

void terminal_putchar(int x, int y, char c, char color)
{
    video_memory[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

void terminal_writechar(char c, char color)
{
    if (c == '\n') {
        terminal_col = 0;
        terminal_row++;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, color);
    terminal_col++;

    if (terminal_col >= VGA_WIDTH) {
        terminal_col = 0;
        terminal_row++;
    }
}

void terminal_init()
{
    video_memory = (uint16_t *) 0xB8000;
    terminal_row = 0;
    terminal_col = 0;

    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

size_t strlen(const char *str)
{
    size_t len = 0;

    while (str[len]) {
        len++;
    }

    return len;
}

void print(const char* string)
{
    size_t len = strlen(string);

    for (size_t i = 0; i < len; i++) {
        terminal_writechar(string[i], 15);
    }
}

void kernel_main() {
    terminal_init();

    print("Hello, World!\ntest");

    // inti the heap
    kheap_init();

    // Init interrupt descriptor table
    idt_init();

    // Enable the systeme interrupts
    enable_interrupts();
}
