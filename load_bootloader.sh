#!/bin/bash

# Compile bootloader
nasm -f bin bootloader/bootloader1.asm -o bootloader/bootloader1.bin

# Create floppy disk image
dd if=/dev/zero of=VMs/floppy.img bs=512 count=2880
dd if=bootloader/bootloader1.bin of=VMs/floppy.img conv=notrunc
