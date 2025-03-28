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

void *elf_memory(struct elf_file *file);
struct elf32_phdr *elf_program_header(struct elf_header *header, int index);
struct elf32_shdr *elf_section(struct elf_header *header, int index);
void *elf_phdr_phys_address(struct elf_file *file, struct elf32_phdr *phdr);

struct elf_header *elf_header(struct elf_file *file);
struct elf32_shdr *elf_sheader(struct elf_header *header);
struct elf32_phdr *elf_pheader(struct elf_header *header);

int elf_load(const char *filename, struct elf_file **file_out);
void elf_close(struct elf_file *file);

# endif // ELF_LOADER_H