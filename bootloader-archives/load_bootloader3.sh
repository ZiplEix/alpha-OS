#!/bin/bash

dd if=/dev/zero of=VMs/floppy.img bs=1024 count=720
nasm -f bin bootloader/bootloader3-1.asm -o bootloader/bootloader3-1.bin
dd if=bootloader/bootloader3-1.bin of=VMs/floppy3.img conv=notrunc

nasm -f bin bootloader/bootloader3-2.asm -o bootloader/bootloader3-2.bin
dd if=bootloader/bootloader3-2.bin of=VMs/floppy3.img bs=512 seek=1 conv=notrunc
