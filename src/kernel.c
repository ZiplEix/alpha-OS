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
#include "disk/streamer.h"
#include "fs/file.h"
#include "gdt/gdt.h"
#include "config.h"
#include "memory/memory.h"
#include "task/tss.h"

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

void panic(const char *message)
{
    print("Kernel panic: ");
    print(message);
    print("\n");

    while (1) {
    }
}

struct tss tss;
struct gdt gtd_real[ALPHAOS_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[ALPHAOS_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00,           .limit = 0x00,        .type = 0x00},                  // NULL Segment
    {.base = 0x00,           .limit = 0xFFFFFFFF,  .type = 0x9A},                  // Kernel Code Segment
    {.base = 0x00,           .limit = 0xFFFFFFFF,  .type = 0x92},                  // Kernel Data Segment
    {.base = 0x00,           .limit = 0xFFFFFFFF,  .type = 0xF8},                  // User Code Segment
    {.base = 0x00,           .limit = 0xFFFFFFFF,  .type = 0xF2},                  // User Data Segment
    {.base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xE9},                  // TSS Segment
};

void kernel_main() {
    terminal_init();

    print("Hello, World!\ntest");

    memset(gtd_real, 0, sizeof(gtd_real));
    gdt_structured_to_gdt(gtd_real, gdt_structured, ALPHAOS_TOTAL_GDT_SEGMENTS);

    // Load the GDT
    gdt_load(gtd_real, sizeof(gtd_real));


    // inti the heap
    kheap_init();

    // Init the file system
    fs_init();

    // search and init disks
    disk_search_and_init();

    // Init interrupt descriptor table
    idt_init();

    // Setup the TSS
    memset(&tss, 0, sizeof(tss));
    tss.esp0 = 0x600000;
    tss.ss0 = KERNEL_DATA_SELECTOR;

    // LOad the TSS
    tss_load(0x28);

    // Setup oaging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // Switch to the kernel paging chunk
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    // Enable paging
    enable_paging();

    // Enable the systeme interrupts
    enable_interrupts();

    int fd = fopen("0:/hello.txt", "r");
    if (fd) {
        struct file_stat s;
        fstat(fd, &s);
        fclose(fd);

        print("File size: ");
    }

    while (1) {
    }
}
