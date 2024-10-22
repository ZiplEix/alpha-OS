
# vars and consts
STAGE1_BIN_NAME = bootloader/stage1/Boot1.bin
STAGE2_BIN_NAME = bootloader/stage2/KRNLDR.SYS
IMAGE_NAME = floppy.img
IMAGE_PATH = VMs/${IMAGE_NAME}
WINDOWS_IMAGE_PATH_ABSOLUTE = /mnt/c/Users/leroy/Documents/OS

# targets
all: build_bootloader build_kernel build_image run

build_bootloader:
	cd bootloader && $(MAKE)

build_kernel:
	cd Kernel && $(MAKE)

build_image:
	dd if=/dev/zero of=${IMAGE_PATH} bs=512 count=2880
	dd if=${STAGE1_BIN_NAME} of=${IMAGE_PATH} bs=512 count=1 conv=notrunc
	mcopy -i "${IMAGE_PATH}" "${STAGE2_BIN_NAME}" ::"KRNLDR.SYS"
	mcopy -i "${IMAGE_PATH}" "Kernel/KERNEL.SYS" ::"KERNEL.SYS"

run:
	@echo "Launching QEMU..."
	@qemu-system-x86_64 -drive format=raw,file=${IMAGE_PATH},if=floppy -boot a -net none

debug: build_bootloader
	@./debug.sh ${IMAGE_PATH} ${WINDOWS_IMAGE_PATH_ABSOLUTE}

clean:
	@echo "Cleaning up..."
	@rm -f ${STAGE1_BIN_NAME} ${STAGE2_BIN_NAME}

fclean: clean
	@echo "Cleaning up all..."
	@rm -f ${IMAGE_PATH}
	@rm -f ${IMAGE_PATH}.lock
	@rm -f ${WINDOWS_IMAGE_PATH_ABSOLUTE}/$(IMAGE_NAME)

.PHONY: all build_bootloader build_image build_kernel run debug clean fclean
