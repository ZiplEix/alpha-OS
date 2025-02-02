#ifndef KERNEL_H
#define KERNEL_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define ALPHAOS_MAX_PATH 108

void kernel_main();
void print(const char* string);

#endif // KERNEL_H