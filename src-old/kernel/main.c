#include <system.h>
#include <drivers.h>
#include <kernlib.h>

void panic(char* text)
{
	settextcolor(0xFF, 0x01);
	cls();
	puts("Panic: ");
	puts(text);
	puts("\nSystem Halted!\n");
	for (;;);
}

/* We will use this later on for reading from the I/O ports to get data
*  from devices such as the keyboard. We are using what is called
*  'inline assembly' in these routines to actually do the work */
unsigned char inportb (unsigned short _port)
{
	unsigned char rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

unsigned short inportw (unsigned short _port)
{
	unsigned short rv;
	__asm__ __volatile__ ("inw %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

/* We will use this to write to I/O ports to send bytes to devices. This
*  will be used in the next tutorial for changing the textmode cursor
*  position. Again, we use some inline assembly for the stuff that simply
*  cannot be done in C */
void outportb (unsigned short _port, unsigned char _data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

void outportw (unsigned short _port, unsigned short _data)
{
	__asm__ __volatile__ ("outw %1, %0" : : "dN" (_port), "a" (_data));
}

int interruptsEnabled = 0;

void enable_interrupts()
{
	// Enable interrupts
	__asm__ __volatile__ ("sti");
	interruptsEnabled = 1;
}

void disable_interrupts()
{
	// Enable interrupts
	__asm__ __volatile__ ("cli");
	interruptsEnabled = 0;
}



int interrupts_enabled()
{
	return interruptsEnabled;
}

/* This is a very simple main() function. All it does is sit in an
*  infinite loop. This will be like our 'idle' loop */
int main()
{
	// Setup GDT, IDT and ISRS
	gdt_install();
	idt_install();
	isrs_install();
	irq_install();
	
	// Setup Basic Memory Paging
	install_paging();
	
	// Init console video
	init_video();
	
	// Print some text
	puts("Blue - Kernel\n-------------------------------------------------\n\n");
	
	settextcolor(0x3E, 0x00);
	puts("GDT Tables initialized...\n");
	puts("IDT Tables initialized...\n");
	puts("ISRS Entries initialized...\n");
	puts("IRQ Entries initialized...\n");
	puts("Basic Memory Paging initialized...\n");
	puts("TTY Video Initialized...\n");
	
	timer_install();
	puts("Programmable Interval Timer Initialized...\n");
	
	keyboard_install();
	puts("Keyboard IRQ handler installed...\n");
	
	puts("Detecting floppy drive(s)...\n");
	settextcolor(0xFF, 0x00);
	FLP_detect();
	settextcolor(0x3E, 0x00);
	
	puts("Probing for IDE drive(s)...\n");
	settextcolor(0xFF, 0x00);
	//IDE_SetDebugging(1);
	Init_IDE();
	settextcolor(0x3E, 0x00);
	
	puts("Retrieving system time...\n");
	settextcolor(0xFF, 0x00);
	kprintf("Current Time: %d:%d:%d %d/%d/%d\n", CMOS_GetHours(), CMOS_GetMinutes(), CMOS_GetSeconds(), \
			CMOS_GetDateMonth(), CMOS_GetDateDay(), CMOS_GetDateYear());
	settextcolor(0x3E, 0x00);
	
	puts("Simple Kernel Shell started...\n\n");
	settextcolor(0xFF, 0x00);
	Intall_SShell();
	
	enable_interrupts();
	
	// Hang
	for (;;);
}
