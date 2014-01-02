all: boot.bin

boot.bin: boot.asm
	nasm -o boot.bin boot.asm

clean:
	rm -f bootloader.bin
