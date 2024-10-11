#!/bin/bash

get_highest_version() {
    ls bootloader/bootloader*.asm | sed 's/[^0-9]*\([0-9]\+\)[^0-9]*/\1/' | sort -n | tail -n 1
}

if [ -n "$1" ]; then
    VERSION="$1"
else
    VERSION=$(get_highest_version)
fi

# Construct the bootloader file names
BOOTLOADER_ASM="bootloader/bootloader${VERSION}.asm"
BOOTLOADER_BIN="bootloader/bootloader${VERSION}.bin"
FLP_IMG="VMs/floppy${VERSION}.img"

# Check if the specified bootloader file exists
if [ ! -f "$BOOTLOADER_ASM" ]; then
    echo "Bootloader version ${VERSION} not found."
    exit 1
fi

# Compile bootloader
nasm -f bin "$BOOTLOADER_ASM" -o "$BOOTLOADER_BIN"

# Create floppy disk image
dd if=/dev/zero of="$FLP_IMG" bs=512 count=2880
dd if="$BOOTLOADER_BIN" of="$FLP_IMG" conv=notrunc

echo "Floppy disk image created: $FLP_IMG"
