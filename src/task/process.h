#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#include "config.h"

struct process {
    // The process ID
    uint16_t id;

    char filename[ALPHAOS_MAX_PATH_LENGTH];

    // The main process task
    struct task *task;

    // The memory allocations of the process
    void *allocations[ALPHAOS_MAX_PROGRAM_ALLOCATIONS];

    // The physical pointer to the process memory
    void *ptr;

    // The physical pointer to the stack memory
    void *stack;

    // The size of the data pointde to by "ptr"
    uint32_t size;

    struct keyboard_buffer {
        char buffer[ALPHAOS_KEYBOARD_BUFFER_SIZE];
        uint32_t head;
        uint32_t tail;
    } keyboard;
};

int process_load(const char *filename, struct process **process);
int process_load_for_slot(const char *filename, struct process **process, int process_slot);
struct process *process_current();
struct process *process_get(int process_id);

#endif // PROCESS_H