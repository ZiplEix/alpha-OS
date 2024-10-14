#!/bin/sh

STAGE1=$1
STAGE2=$2
STAGE1_BIN=$3
STAGE2_BIN=$4
IMAGE=$5
STAGE2_FILE_NAME=$(basename $STAGE2_BIN)

echo "== Building bootloader =="
nasm -f bin $STAGE1 -o $STAGE1_BIN

# check if the stage1 is 512 bytes
echo "== Checking if stage1 is 512 bytes =="
if [ $(stat -c %s $STAGE1_BIN) -ne 512 ]; then
    echo "\033[0;31mError: stage1 is not 512 bytes\033[0m"
    exit 1
else
    echo "\033[0;32mstage1 is 512 bytes\033[0m"
fi
echo ""

echo "== Building stage2 =="
nasm -f bin $STAGE2 -o $STAGE2_BIN
echo ""

echo "== Creating image =="
dd if=/dev/zero of=$IMAGE bs=512 count=2880
dd if=$STAGE1_BIN of=$IMAGE bs=512 count=1 conv=notrunc
dd if=$STAGE1_BIN of=$IMAGE bs=1 skip=62 seek=62 conv=notrunc
echo " - $STAGE1 added to image"

mcopy -i $IMAGE $STAGE2_BIN ::$STAGE2_FILE_NAME
echo " - $STAGE2_BIN added to image"

echo "\033[0;32m== Image successfuly created ==\033[0m"
