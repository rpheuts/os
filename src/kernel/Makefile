all: stage1.bin

entry.bin: entry.asm
	nasm entry.asm -o entry.bin

main.o: main.c
	/opt/cross/bin/i586-elf-gcc -c main.c -o main.o -std=gnu99 -ffreestanding -Wall -Wextra

main.bin: main.o
	/opt/cross/bin/i586-elf-ld main.o -o main.bin --oformat=binary -T linker.ld

stage1.bin: main.bin entry.bin

clean:
	rm -f entry.bin
	rm -f main.o
	rm -f main.bin
