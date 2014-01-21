GCC64 = /opt/cross/bin/x86_64-pc-elf-gcc -c -m64 -ffreestanding
LD64 = /opt/cross/bin/x86_64-pc-elf-ld -melf_x86_64 -T linker.ld -nostdlib -e main -Ttext 0x10400
OBJCOPY64 = objcopy -j .text -j .data -j .rodata -j .bss -S -O binary
NASM64 = nasm -f elf64

all: kernel.bin

entry.o: entry.asm
	$(NASM64) -o entry.o

main.o: main.c
	$(GCC64) main.c -o main.o

kernel.elf: main.o entry.o
	$(LD64) main.o entry.o -o kernel.elf

kernel.bin: kernel.elf
	$(OBJCOPY64) kernel.elf kernel.bin

stage1.bin: main.bin entry.bin

clean:
	rm -f entry.bin
	rm -f main.o
	rm -f kernel.bin
