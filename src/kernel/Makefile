GCC64 = /opt/cross/bin/x86_64-pc-elf-gcc -c -m64 -ffreestanding
LD64 = /opt/cross/bin/x86_64-pc-elf-ld -melf_x86_64 -T linker.ld -nostdlib -e start -Ttext 0x0
OBJCOPY64 = objcopy -j .text -j .data -j .rodata -j .bss -S -O binary
NASM64 = nasm -f elf64

all: kernel.bin

entry.o: entry.asm
	$(NASM64) entry.asm -o entry.o

main.o: main.c
	$(GCC64) main.c -o main.o

kernel.elf: main.o entry.o
	$(LD64) entry.o main.o -o kernel.elf

kernel.bin: kernel.elf
	$(OBJCOPY64) kernel.elf kernel.bin

clean:
	rm -f entry.o
	rm -f main.o
	rm -f kernel.elf
	rm -f kernel.bin
