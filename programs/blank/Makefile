NAME := blank.elf

all:
	@mkdir -p ./build
	nasm -f elf ./blank.asm -o ./build/blank.o
	i686-elf-gcc -g -T ./linker.ld -o ./$(NAME) -ffreestanding -O0 -nostdlib -fpic ./build/blank.o

clean:
	@rm -rf ./build

fclean: clean
	@rm -f ./$(NAME)

re: fclean all
