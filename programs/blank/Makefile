all:
	@mkdir -p ./build
	nasm -f elf ./blank.asm -o ./build/blank.o
	i686-elf-gcc -g -T ./linker.ld -o ./blank.bin -ffreestanding -O0 -nostdlib -fpic ./build/blank.o

clean:
	@rm -rf ./build

fclean: clean
	@rm -f ./blank.bin

re: fclean all
