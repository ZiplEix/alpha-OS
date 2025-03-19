NAME := blank.elf

CC		= i686-elf-gcc
AS		= nasm
LD 		= i686-elf-ld

SRC_DIR	= .
BUILD   = build
BIN    	= bin

INCLUDES = -I$(SRC_DIR) -I../stdlib/src

FLAGS    = -g

NASMFLAGS_ELF = -f elf -g
NASMFLAGS_BIN = -f bin -g

FILES = $(BUILD)/blank.o

all: $(FILES)
	i686-elf-gcc -g -T ./linker.ld -o ./$(NAME) -ffreestanding -O0 -nostdlib -fpic $(FILES) ../stdlib/stdlib.elf

$(BUILD)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@

clean:
	@rm -rf ./build

fclean: clean
	@rm -f ./$(NAME)

re: clean fclean all
