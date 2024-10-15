# AlphaOS

AlphaOS is a personal project I'm undertaking to delve into the intricacies of operating system development. My goal is to create a full-fledged 32-bit, command-line-driven operating system without a graphical interface (for now). The project is still in its early stages, and I'm working on it in my free time.

**Motivation**

This project is driven by my desire to:

* Gain a deeper understanding of computer architecture and operating system design principles.
* Explore the challenges and rewards of building a complex system from scratch.
* Experiment with various technologies and techniques used in OS development.
* Ultimately, create a functional and educational operating system that can be used for learning and experimentation.

## Summary

This document provides an overview of the AlphaOS project, focusing on the recently completed bootloader stage. The summary outlines the key sections:

* [Introduction](#alphaos)
* [Summary](#summary)
* [Bootloader](#bootloader)

## Bootloader

The AlphaOS bootloader is responsible for initializing the hardware upon system startup and preparing the environment for the kernel to take over. Here's a breakdown of its functionalities:

### Responsibilities

* **POST (Power-On Self-Test):** Performs basic hardware diagnostics to ensure the system's functionality.
* **Loading the Kernel:** Locates and loads the kernel image from a designated storage device (e.g., hard disk or floppy disk) into memory.
* **Memory Management:** Sets up the initial memory layout for the kernel, potentially including paging and segmentation.
* **Control Transfer:** Hands over control to the loaded kernel, allowing it to take over system operations.

### Implementation Details

* The bootloader is written in assembly language for direct hardware interaction and low-level control.
* Stage 1 (bootloader/stage1.asm) resides in the first sector of the boot device and initiates the loading process.
* Stage 2 (bootloader/stage2.asm) performs more complex tasks like locating and loading the kernel image.

### Code Explanation

The bootloader is divided in 2 stages :

#### Stage 1

- Initialization:
    - Sets up the segment registers to point to the bootloader's location at 0x07C0.
    - Creates a stack for the bootloader to use.
    - Initializes the BIOS video mode for text output.
- BIOS Parameter Block (BPB):
    - Defines the BPB, which contains information about the disk's geometry and file system.
- Root Directory Loading:
    - Calculates the location of the root directory based on the BPB.
    - Reads the root directory entries into memory.
- Stage 2 Search:
    - Scans the root directory for a file named "KRNLDR.SYS" (the stage 2 bootloader).
    - If found, proceeds to load stage 2.

#### Stage 2

- Initialization:
    - Sets up the segment registers for stage 2.
- Loading Kernel:
    - Reads the kernel image from the disk using the FAT file system.
Transfers control to the loaded kernel.

### Key Functions

- Print: Displays text on the screen using the BIOS video interrupt.
- ReadSectors: Reads a series of sectors from the disk.
- ClusterLBA: Converts a cluster number to a logical block address (LBA).
- LBACHS: Converts an LBA to a CHS (cylinder, head, sector) address.

### Additional notes

- The bootloader uses the BIOS interrupts to interact with the hardware, such as reading from the disk and displaying text. Available only in real mode (16 bit).
- The code includes error handling mechanisms to detect and handle potential issues during the boot process.
- The bootloader is designed to be self-contained and does not rely on external libraries or modules.
- The code is well-commented to enhance readability and understanding.
