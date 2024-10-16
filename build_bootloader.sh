#!/bin/sh

print_message() {
    if [ "$QUIET" != "true" ]; then
        echo "$1"
    fi
}

QUIET=false
if [ "$1" = "-q" ]; then
    QUIET=true
    shift
fi

STAGE1=$1
STAGE2=$2
STAGE1_BIN=$3
STAGE2_BIN=$4
IMAGE=$5
STAGE2_FILE_NAME=$(basename "$STAGE2_BIN")

print_message "=============================="
print_message "      Building Bootloader      "
print_message "=============================="
nasm -f bin "$STAGE1" -o "$STAGE1_BIN"

# Vérifier si le stage1 fait 512 octets
print_message "== Checking if stage1 is 512 bytes =="
if [ "$(stat -c %s "$STAGE1_BIN")" -ne 512 ]; then
    print_message "\033[0;31mError: stage1 is not 512 bytes\033[0m"
    exit 1
else
    print_message "\033[0;32mstage1 is 512 bytes\033[0m"
fi
print_message ""

print_message "== Building stage2 =="
nasm -f bin "$STAGE2" -o "$STAGE2_BIN"
print_message ""

print_message "== Creating image =="
dd if=/dev/zero of="$IMAGE" bs=512 count=2880
dd if="$STAGE1_BIN" of="$IMAGE" bs=512 count=1 conv=notrunc
print_message " - $STAGE1 added to image"

mcopy_cmd="mcopy -i \"$IMAGE\" \"$STAGE2_BIN\" ::\"$STAGE2_FILE_NAME\""
if [ "$QUIET" = "true" ]; then
    eval "$mcopy_cmd > /dev/null 2>&1"
else
    eval "$mcopy_cmd"
fi

print_message " - $STAGE2_BIN added to image"

print_message "\033[0;32m== Image successfully created ==\033[0m"
