#ifndef CONFIG_H
#define CONFIG_H

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

#define ALPHAOS_TOTAL_INTERRUPTS 512

// 100MB heap size
#define ALPHAOS_HEAP_SIZE_BYTES 104857600
#define ALPHAOS_HEAP_BLOCK_SIZE 4096
#define ALPHAOS_HEAP_ADDRESS 0x01000000
#define ALPHAOS_HEAP_TABLE_ADDRESS 0x00007E00

#define ALPHAOS_SECTOR_SIZE 512

#define ALPHAOS_MAX_FILESYSTEMS 12
#define ALPHAOS_MAX_FILEDESCRIPTORS 512

#define ALPHAOS_MAX_PATH_LENGTH 108

#endif // CONFIG_H