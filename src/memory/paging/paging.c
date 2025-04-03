#include "paging.h"
#include "memory/heap/kheap.h"
#include "status.h"

void paging_load_directory(uint32_t *directory);

static uint32_t *current_directory = 0;

struct paging_4gb_chunk *paging_new_4gb(uint8_t flags)
{
    uint32_t *directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    int offset = 0;

    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
        uint32_t *entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);

        for (int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; j++) {
            entry[j] = (offset + (j * PAGING_PAGE_SIZE)) | flags;
        }

        offset += PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE;
        directory[i] = ((uint32_t)entry) | flags | PAGING_IS_WRITABLE;
    }

    struct paging_4gb_chunk *chunk_4gb = kzalloc(sizeof(struct paging_4gb_chunk));

    chunk_4gb->directory_entry = directory;

    return chunk_4gb;
}

void paging_switch(struct paging_4gb_chunk *directory)
{
    paging_load_directory(directory->directory_entry);
    current_directory = directory->directory_entry;
}

void paging_free_4gb(struct paging_4gb_chunk *chunk)
{
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
        uint32_t entry = chunk->directory_entry[i];
        uint32_t *table = (uint32_t *)(entry & 0xFFFFF000);

        kfree(table);
    }

    kfree(chunk->directory_entry);
    kfree(chunk);
}

uint32_t *paging_4gb_chunk_get_directory(struct paging_4gb_chunk *chunk)
{
    return chunk->directory_entry;
}

bool paging_is_aligned(void *addr)
{
    return ((uint32_t)addr % PAGING_PAGE_SIZE) == 0;
}

int paging_get_indexes(void *virtual_address, uint32_t *directory_index_out, uint32_t *table_index_out)
{
    int res = 0;

    if (!paging_is_aligned(virtual_address)) {
        res = -EINVARG;
        goto out;
    }

    *directory_index_out = ((uint32_t)virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    *table_index_out = ((uint32_t)virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE)) / PAGING_PAGE_SIZE;

out:
    return res;
}

void *paging_align_address(void *ptr)
{
    if ((uint32_t)ptr % PAGING_PAGE_SIZE) {
        return (void *)((uint32_t)ptr + PAGING_PAGE_SIZE - ((uint32_t)ptr % PAGING_PAGE_SIZE));
    }

    return ptr;
}

void *paging_align_to_lower_page(void *addr)
{
    uint32_t _addr = (uint32_t)addr;
    _addr -= (_addr % PAGING_PAGE_SIZE);
    return (void *)_addr;
}

int paging_map(struct paging_4gb_chunk *directory, void *virtual_address, void *physical_address, int flags)
{
    if (((unsigned int)virtual_address % PAGING_PAGE_SIZE) || ((unsigned int)physical_address % PAGING_PAGE_SIZE)) {
        return -EINVARG;
    }

    return paging_set(directory->directory_entry, virtual_address, (uint32_t)physical_address | flags);
}

int paging_map_range(struct paging_4gb_chunk *directory, void *virtual_address, void *physical_address, int count, int flags)
{
    int res = 0;

    for (int i = 0; i < count; i++) {
        res = paging_map(directory, virtual_address, physical_address, flags);
        if (res < 0) {
            break;
        }

        virtual_address += PAGING_PAGE_SIZE;
        physical_address += PAGING_PAGE_SIZE;
    }

    return res;
}

int paging_map_to(struct paging_4gb_chunk *directory, void *virtual_adress, void *physical_address, void *physical_end_address, int flags)
{
    int res = 0;
    if ((uint32_t)virtual_adress % PAGING_PAGE_SIZE) {
        res = -EINVARG;
        goto out;
    }

    if ((uint32_t)physical_address % PAGING_PAGE_SIZE) {
        res = -EINVARG;
        goto out;
    }

    if ((uint32_t)physical_end_address % PAGING_PAGE_SIZE) {
        res = -EINVARG;
        goto out;
    }

    if ((uint32_t)physical_end_address < (uint32_t)physical_address) {
        res = -EINVARG;
        goto out;
    }

    uint32_t total_bytes = (uint32_t)physical_end_address - (uint32_t)physical_address;
    int total_pages = total_bytes / PAGING_PAGE_SIZE;
    res = paging_map_range(directory, virtual_adress, physical_address, total_pages, flags);

out:
    return res;
}

int paging_set(uint32_t *directory, void *virtual_address, uint32_t value)
{
    if (!paging_is_aligned(virtual_address)) {
        return -EINVARG;
    }

    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    int res = paging_get_indexes(virtual_address, &directory_index, &table_index);

    if (res < 0) {
        return res;
    }

    uint32_t entry = directory[directory_index];
    uint32_t *table = (uint32_t *)(entry & 0xFFFFF000);

    table[table_index] = value;

    return res;
}

void *paging_get_physical_address(uint32_t *directory, void *virtual_address)
{
    void *new_virtual_address = (void *)paging_align_to_lower_page(virtual_address);
    void *difference = (void *)((uint32_t)virtual_address - (uint32_t)new_virtual_address);
    return (void *)((paging_get(directory, new_virtual_address) & 0xFFFFF000) + (uint32_t)difference);
}

uint32_t paging_get(uint32_t *directory, void *virtual_address)
{
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    paging_get_indexes(virtual_address, &directory_index, &table_index);
    uint32_t entry = directory[directory_index];
    uint32_t *table = (uint32_t *)(entry & 0xFFFFF000);

    return table[table_index];
}
