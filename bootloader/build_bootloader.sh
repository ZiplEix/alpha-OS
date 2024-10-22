#!/bin/sh

# print_message() {
#     if [ "$QUIET" != "true" ]; then
#         echo "$1"
#     fi
# }

# QUIET=false
# if [ "$1" = "-q" ]; then
#     QUIET=true
#     shift
# fi

STAGE1=$1
STAGE2=$2
STAGE1_BIN=$3
STAGE2_BIN=$4
IMAGE=$5
STAGE2_FILE_NAME=$(basename "$STAGE2_BIN")

echo "STAGE1: $STAGE1"
echo "STAGE2: $STAGE2"
echo "STAGE1_BIN: $STAGE1_BIN"
echo "STAGE2_BIN: $STAGE2_BIN"
echo "IMAGE: $IMAGE"
echo "STAGE2_FILE_NAME: $STAGE2_FILE_NAME"

exit 0

echo "=============================="
echo "      Building Bootloader      "
echo "=============================="
nasm -f bin "$STAGE1" -o "$STAGE1_BIN"

# Vérifier si le stage1 fait 512 octets
echo "== Checking if stage1 is 512 bytes =="
if [ "$(stat -c %s "$STAGE1_BIN")" -ne 512 ]; then
    echo "\033[0;31mError: stage1 is not 512 bytes\033[0m"
    exit 1
else
    echo "\033[0;32mstage1 is 512 bytes\033[0m"
fi
echo ""

echo "== Building stage2 =="
nasm -f bin "$STAGE2" -o "$STAGE2_BIN"
echo ""

echo "== Creating image =="
dd if=/dev/zero of="$IMAGE" bs=512 count=2880
dd if="$STAGE1_BIN" of="$IMAGE" bs=512 count=1 conv=notrunc
echo " - $STAGE1 added to image"

mcopy_cmd="mcopy -i \"$IMAGE\" \"$STAGE2_BIN\" ::\"$STAGE2_FILE_NAME\""
if [ "$QUIET" = "true" ]; then
    eval "$mcopy_cmd > /dev/null 2>&1"
else
    eval "$mcopy_cmd"
fi

echo " - $STAGE2_BIN added to image"

echo "\033[0;32m== Image successfully created ==\033[0m"
