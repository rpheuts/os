K_DIR= ./kernel/*.c
D_DIR= ./drivers/
INCLUDE= ./kernel/include/
CC= gcc
GCCFLAGS= -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o
SRCS= main.c screen.c gdt.c idt.c isrs.c timer.c kb.c mem.c sshell.c kernlib.c
OBJS= main.o screen.o gdt.o idt.o isrs.o timer.o kb.o mem.o sshell.o kernlib.o

blaat:
	$(CC) $(GCCFLAGS) $*.c

clean:
	rm -f *.o *.dll