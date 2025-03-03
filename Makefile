CC		= i686-elf-gcc
AS		= nasm
LD 		= i686-elf-ld

SRC_DIR	= src
BUILD   = build
BIN    	= bin

INCLUDES = -I$(SRC_DIR)

FLAGS    = -g -ffreestanding -falign-jumps -falign-functions -falign-labels \
           -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions \
           -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp \
           -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0

NASMFLAGS_ELF = -f elf -g
NASMFLAGS_BIN = -f bin -g

FILES = $(BUILD)/kernel.asm.o $(BUILD)/kernel.o \
        $(BUILD)/disk/disk.o $(BUILD)/disk/streamer.o \
        $(BUILD)/task/tss.asm.o $(BUILD)/task/task.o $(BUILD)/task/process.o $(BUILD)/task/task.asm.o \
        $(BUILD)/fs/pparser.o $(BUILD)/fs/file.o \
        $(BUILD)/fs/fat/fat16.o \
        $(BUILD)/string/string.o \
        $(BUILD)/idt/idt.asm.o $(BUILD)/idt/idt.o \
        $(BUILD)/io/io.asm.o \
        $(BUILD)/gdt/gdt.o $(BUILD)/gdt/gdt.asm.o \
		$(BUILD)/isr80h/isr80h.o $(BUILD)/isr80h/misc.o \
        $(BUILD)/memory/memory.o \
        $(BUILD)/memory/heap/heap.o $(BUILD)/memory/heap/kheap.o \
        $(BUILD)/memory/paging/paging.o $(BUILD)/memory/paging/paging.asm.o

all: $(BIN)/boot.bin $(BIN)/kernel.bin programs
	@rm -rf $(BIN)/os.bin
	@dd if=$(BIN)/boot.bin >> $(BIN)/os.bin
	@dd if=$(BIN)/kernel.bin >> $(BIN)/os.bin
	@dd if=/dev/zero bs=1048576 count=16 >> $(BIN)/os.bin
	@sudo mount -t vfat $(BIN)/os.bin /mnt/d
	@sudo cp ./hello.txt /mnt/d
	@sudo cp ./programs/blank/blank.bin /mnt/d
	@sudo umount /mnt/d

$(BIN)/kernel.bin: $(FILES)
	$(LD) -g -relocatable $(FILES) -o $(BUILD)/kernelfull.o
	$(CC) $(FLAGS) -T $(SRC_DIR)/linker.ld -o $(BIN)/kernel.bin -ffreestanding -O0 -nostdlib $(BUILD)/kernelfull.o

$(BIN)/boot.bin: $(SRC_DIR)/boot/boot.asm
	$(AS) $(NASMFLAGS_BIN) $< -o $@

$(BUILD)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@

$(BUILD)/%.asm.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	$(AS) $(NASMFLAGS_ELF) $< -o $@

clean:
	@rm -rf $(BIN)/boot.bin $(BIN)/kernel.bin $(FILES) $(BUILD)/kernelfull.o

fclean: clean programs_clean
	@rm -rf $(BIN)/os.bin

re: fclean all

run:
	@qemu-system-i386 -hda ./bin/os.bin

programs:
	$(MAKE) -C programs/blank all

programs_clean:
	$(MAKE) -C programs/blank clean

debug:
# user land : *0x400000
# kernel land : 0x100000
	gdb -q -ex "add-symbol-file ./build/kernelfull.o 0x100000" -ex "target remote | qemu-system-i386 -hda ./bin/os.bin -S -gdb stdio" -ex "break *0x400000"

.PHONY: all clean fclean re programs programs_clean run debug
