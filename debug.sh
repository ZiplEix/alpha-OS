#!/bin/sh

IMAGE_NAME=$1
WINDOWS_IMAGE_PATH_ABSOLUTE=$2

echo "=============================="
echo "   Launching BOCHS in Debug   "
echo "=============================="
cp ${IMAGE_NAME} ${WINDOWS_IMAGE_PATH_ABSOLUTE}
echo "Successfully copied image to Windows path: [${WINDOWS_IMAGE_PATH_ABSOLUTE}]"
echo ""
echo "To launch BOCHS, run the following command:"
echo ""
echo "   /mnt/c/Program\ Files/Bochs-2.8/bochsdbg.exe -f bochsrc.txt"
echo "   <bochs:1> b 0x7c00   # Set breakpoint at 0x7c00 (start of bootloader)"
echo "   <bochs:2> c          # Continue execution"
echo "   <bochs:3> b 0x500    # Set breakpoint at 0x500 (start of stage2)"
echo "   <bochs:4> c          # Continue execution"
echo ""
echo "=============================="
