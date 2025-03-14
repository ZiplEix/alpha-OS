#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include <stdint.h>
#include <stddef.h>

#include "elf.h"
#include "config.h"

struct elf_file {
    char filename[ALPHAOS_MAX_PATH_LENGTH];

    int in_memory_size;

    // The physical memory address that this elf file os loaded at
    void *elf_memory;

    // The virtual base address of thes binary
    void *virtual_base_address;

    // The ending virtual address
    void *virtual_end_address;

    // The physical base address of the binary
    void *physical_base_address;

    // The physical end address of the binary
    void *physical_end_address;
};

void *elf_virtual_base(struct elf_file *file);
void *elf_virtual_end(struct elf_file *file);
void *elf_phys_base(struct elf_file *file);
void *elf_phys_end(struct elf_file *file);

int elf_load(const char *filename, struct elf_file **file_out);
void elf_close(struct elf_file *file);

# endif // ELF_LOADER_H