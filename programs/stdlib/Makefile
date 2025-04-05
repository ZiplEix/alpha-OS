NAME :=	stdlib.elf

CROSS 	= ../../opt/cross/bin/
CC		= $(CROSS)i686-elf-gcc
AS		= nasm
LD 		= $(CROSS)i686-elf-ld

SRC_DIR	= ./src
BUILD   = build
BIN    	= bin

INCLUDES = -I$(SRC_DIR)

FLAGS    = -g -falign-jumps -falign-functions -falign-labels \
           -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions \
           -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp \
           -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0

NASMFLAGS_ELF = -f elf -g
NASMFLAGS_BIN = -f bin -g

FILES = $(BUILD)/start.asm.o $(BUILD)/start.o \
		$(BUILD)/alphaos.asm.o $(BUILD)/alphaos.o \
		$(BUILD)/stdlib.o \
		$(BUILD)/stdio.o \
		$(BUILD)/string.o \
		$(BUILD)/memory.o \

all: $(FILES)
	$(LD) -m elf_i386 -relocatable $(FILES) -o $(NAME)

$(BUILD)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@

$(BUILD)/%.asm.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	$(AS) $(NASMFLAGS_ELF) $< -o $@

clean:
	@rm -rf ./build

fclean: clean
	@rm -f ./$(NAME)

re: clean fclean all
