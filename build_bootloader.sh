#!/bin/sh

STAGE1="Boot1.bin"
STAGE2="KRNLDR.SYS"
IMAGE="VMs/floppy.img"

echo "== Building bootloader =="
nasm -f bin ./bootloader/stage1.asm -o bootloader/$STAGE1

# check if the stage1 is 512 bytes
echo "== Checking if stage1 is 512 bytes =="
if [ $(stat -c %s bootloader/$STAGE1) -ne 512 ]; then
    echo "\033[0;31mError: stage1 is not 512 bytes\033[0m"
    exit 1
else
    echo "\033[0;32mstage1 is 512 bytes\033[0m"
fi
echo ""

echo "== Building stage2 =="
nasm -f bin ./bootloader/stage2.asm -o bootloader/$STAGE2
echo ""

echo "== Creating image =="
dd if=/dev/zero of=$IMAGE bs=512 count=2880
dd if=bootloader/$STAGE1 of=$IMAGE bs=512 count=1 conv=notrunc
dd if=bootloader/$STAGE1 of=$IMAGE bs=1 skip=62 seek=62 conv=notrunc
echo " - $STAGE1 added to image"

mcopy -i $IMAGE bootloader/$STAGE2 ::$STAGE2
echo " - $STAGE2 added to image"

echo "\033[0;32m== Image successfuly created ==\033[0m"
