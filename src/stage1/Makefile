all: stage1.bin

entry.o: entry.asm
	nasm -f elf entry.asm -o entry.o

main.o: main.c
	~/opt/cross/bin/i586-elf-gcc -c main.c -o main.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

stage1.bin: main.o entry.o
	echo "bla";

clean:
	rm -f entry.o
	rm -f main.o
	rm -f stage1.bin
