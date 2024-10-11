#!/bin/sh

STAGE1="Boot1.bin"
STAGE2="KRNLDR.SYS"
IMAGE="VMs/floppy.img"

nasm -f bin ./bootloader/stage1.asm -o bootloader/$STAGE1
nasm -f bin ./bootloader/stage2.asm -o bootloader/$STAGE2

# ./bootloader/build.sh $STAGE1 $STAGE2

dd if=/dev/zero of=$IMAGE bs=512 count=720
dd if=bootloader/$STAGE1 of=$IMAGE bs=512 count=1 conv=notrunc
dd if=bootloader/$STAGE1 of=$IMAGE bs=1 skip=62 seek=62 conv=notrunc

mcopy -i $IMAGE bootloader/$STAGE2 ::$STAGE2
