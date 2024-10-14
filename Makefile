
# vars and consts
STAGE1 = "bootloader/stage1.asm"
STAGE2 = "bootloader/stage2.asm"
STAGE1_BIN_NAME = "bootloader/Boot1.bin"
STAGE2_BIN_NAME = "bootloader/KRNLDR.SYS"
IMAGE_NAME = "VMs/floppy.img"

# targets
all: build run

build_bootloader:
	@echo "Building bootloader..."
	@./build_bootloader.sh ${STAGE1} ${STAGE2} ${STAGE1_BIN_NAME} ${STAGE2_BIN_NAME} ${IMAGE_NAME}

run:
	@echo "Launching QEMU..."
	@qemu-system-x86_64 -drive format=raw,file=${IMAGE_NAME},if=floppy -boot a -net none

.PHONY: all build run

