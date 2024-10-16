
# vars and consts
STAGE1 = "bootloader/stage1.asm"
STAGE2 = "bootloader/stage2.asm"
STAGE1_BIN_NAME = "bootloader/Boot1.bin"
STAGE2_BIN_NAME = "bootloader/KRNLDR.SYS"
IMAGE_NAME = "VMs/floppy.img"
WINDOWS_BOCHS_PATH = "/mnt/c/Program\ Files/Bochs-2.8/bochs.exe"
WINDOWS_IMAGE_PATH = C:/Users/leroy/Documents/OS/floppy.img
WINDOWS_IMAGE_PATH_ABSOLUTE = "/mnt/c/Users/leroy/Documents/OS/"

# targets
all: build_bootloader run

build_bootloader:
	@./build_bootloader.sh ${STAGE1} ${STAGE2} ${STAGE1_BIN_NAME} ${STAGE2_BIN_NAME} ${IMAGE_NAME}

run:
	@echo "Launching QEMU..."
	@qemu-system-x86_64 -drive format=raw,file=${IMAGE_NAME},if=floppy -boot a -net none

debug: build_bootloader
	@./debug.sh ${IMAGE_NAME} ${WINDOWS_IMAGE_PATH_ABSOLUTE}

clean:
	@echo "Cleaning up..."
	@rm -f ${STAGE1_BIN_NAME} ${STAGE2_BIN_NAME}

fclean: clean
	@echo "Cleaning up all..."
	@rm -f ${IMAGE_NAME}

.PHONY: all build run debug clean fclean

