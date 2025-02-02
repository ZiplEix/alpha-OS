#include <stdint.h>
#include <stddef.h>

#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "string/string.h"
#include "disk/disk.h"
#include "fs/pparser.h"

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

void print(const char* string)
{
    size_t len = strlen(string);

    for (size_t i = 0; i < len; i++) {
        terminal_writechar(string[i], 15);
    }
}

static struct paging_4gb_chunk *kernel_chunk = 0;

void kernel_main() {
    terminal_init();

    print("Hello, World!\ntest");

    // inti the heap
    kheap_init();

    // search and init disks
    disk_search_and_init();

    // Init interrupt descriptor table
    idt_init();

    // Setup oaging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // Switch to the kernel paging chunk
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    // Enable paging
    enable_paging();

    // Enable the systeme interrupts
    enable_interrupts();

    struct path_root* root_path = pathparser_parse("0:/bin/shell.exe", NULL);
    if(root_path) {
    }
}
