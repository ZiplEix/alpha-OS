NAME := shell.elf

CROSS 	= ../../opt/cross/bin/
CC		= $(CROSS)i686-elf-gcc
AS		= nasm
LD 		= $(CROSS)i686-elf-ld

SRC_DIR	= ./src
BUILD   = build
BIN    	= bin

INCLUDES = -I$(SRC_DIR) -I../stdlib/src

FLAGS    = -g -ffreestanding -falign-jumps -falign-functions -falign-labels \
           -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions \
           -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp \
           -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0

NASMFLAGS_ELF = -f elf -g
NASMFLAGS_BIN = -f bin -g

FILES = $(BUILD)/shell.o

all: $(FILES)
	$(CC) -g -T ./linker.ld -o ./$(NAME) -ffreestanding -O0 -nostdlib -fpic $(FILES) ../stdlib/stdlib.elf

$(BUILD)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@

clean:
	@rm -rf ./build

fclean: clean
	@rm -f ./$(NAME)

re: clean fclean all
