NAME :=	stdlib.elf

CC		= i686-elf-gcc
AS		= nasm
LD 		= i686-elf-ld

SRC_DIR	= ./src
BUILD   = build
BIN    	= bin

INCLUDES = -I$(SRC_DIR)

FLAGS    = -g

NASMFLAGS_ELF = -f elf -g
NASMFLAGS_BIN = -f bin -g

FILES = $(BUILD)/start.o \
		$(BUILD)/alphaos.o \
		$(BUILD)/stdlib.o \
		$(BUILD)/stdio.o \

all: $(FILES)
	$(LD) -m elf_i386 -relocatable $(FILES) -o $(NAME)

$(BUILD)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@

$(BUILD)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	$(AS) $(NASMFLAGS_ELF) $< -o $@

clean:
	@rm -rf ./build

fclean: clean
	@rm -f ./$(NAME)

re: clean fclean all
