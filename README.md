# Alpha OS

Alpha OS is a personal educational project that showcases my learning journey in developing and understanding the workings of a kernel for x86 processors. The produced kernel is not intended for practical use but serves solely as a learning experience.

## Table of Contents

- [Project Overview](#project-overview)
- [Project Architecture](#project-architecture)
- [Getting Started](#getting-started)
- [Building the Project](#building-the-project)
- [Running the Kernel](#running-the-kernel)
- [Contributing](#contributing)

## Project Overview

The goal of Alpha OS is to provide a hands-on approach to understanding the inner workings of an operating system kernel. Through this project, I have gained insights into low-level programming, system calls, memory management, process scheduling, and more.

## Project Architecture

Alpha OS is structured into several key components, each located in their respective directories within the `src` folder:

- **Bootloader** (`src/boot`): Initializes the system and loads the kernel into memory.
- **Kernel**:
  - `src/kernel.asm`: Assembly code for initializing the kernel.
  - `src/kernel.c`: Main C code for the kernel operations.
  - `src/kernel.h`: Header file for kernel definitions.
- **Configuration** (`src/config.h`): Configuration settings for the kernel.
- **Disk Management** (`src/disk`): Handles interactions with disk storage.
- **File System** (`src/fs`): Provides a basic interface for file operations.
- **Global Descriptor Table (GDT)** (`src/gdt`): Manages memory segmentation.
- **Interrupt Descriptor Table (IDT)** (`src/idt`): Handles interrupt vectors.
- **Input/Output Operations** (`src/io`): Manages low-level I/O operations.
- **Memory Management**:
  - `src/memory`: Handles allocation and deallocation of memory.
- **Task Management** (`src/task`): Manages the creation, scheduling, and termination of processes.
- **Utilities**:
  - `src/status.h`: Status codes and utility functions.
  - `src/string`: String manipulation functions.
- **Linker Script** (`src/linker.ld`): Script for linking the kernel.

For more detailed information, you can explore the [src directory](https://github.com/ZiplEix/alpha-OS/tree/redo/src).

## Getting Started

To get started with Alpha OS, you will need the following tools:

- A cross-compiler for building the kernel.
- QEMU or another emulator for testing the kernel.

### Prerequisites

- GCC Cross-Compiler
- Make
- QEMU

### Installation

1. Clone the repository:
    ```sh
    git clone --branch redo https://github.com/ZiplEix/alpha-OS.git
    cd alpha-OS
    ```

2. Set up the cross-compiler:
    Follow the instructions [here](https://wiki.osdev.org/GCC_Cross-Compiler) to set up a cross-compiler.

## Building the Project

To build the project, run the following commands:

```sh
make
```

This will compile the kernel and generate a bootable ISO image.

## Running the Kernel

To run the kernel using QEMU, execute:

```sh
make run
```

This will launch QEMU and boot the generated ISO image.

## Acknowledgment

The majority of the code in Alpha OS is based on the work of Daniel McCarthy and the [OSDev](https://wiki.osdev.org/) wiki. However, all the code present in this repository has been thoroughly understood and assimilated. The documentation, on the other hand, is entirely my own work.

## Contributing

Contributions are welcome! If you have any improvements or fixes, please submit a pull request. For major changes, please open an issue first to discuss what you would like to change.
