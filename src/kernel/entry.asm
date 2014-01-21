
[BITS 64]

SECTION .text

extern main
global start

start:
	call main
	jmp $
