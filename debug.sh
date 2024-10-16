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
echo ""
echo "=============================="
