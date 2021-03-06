#include <system.h>
#include <kernlib.h>

/* This is a simple string array. It contains the message that
*  corresponds to each and every exception. We get the correct
*  message by accessing like:
*  exception_message[interrupt_number] */
unsigned char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint Exception",
    "Into Detected Overflow Exception",
    "Out of Bounds Exception",
    "Invalid Opcode Exception",
    "No Coprocessor Exception",
    "Double Fault Exception",
    "Coprocessor Segment Overrun Exception",
    "Bad TSS Exception",
    "Segment Not Present Exception",
    "Stack Fault Exception",
    "General Protection Fault Exception",
    "Page Fault Exception",
    "Unknown Interrupt Exception",
    "Coprocessor Fault Exception",
    "Alignment Check Exception (486+)",
    "Machine Check Exception (Pentium/586+)",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

/* These are function prototypes for all of the exception
*  handlers: The first 32 entries in the IDT are reserved
*  by Intel, and are designed to service exceptions! */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

/* This is a very repetitive function... it's not hard, it's
*  just annoying. As you can see, we set the first 32 entries
*  in the IDT to the first 32 ISRs. We can't use a for loop
*  for this, because there is no way to get the function names
*  that correspond to that given entry. We set the access
*  flags to 0x8E. This means that the entry is present, is
*  running in ring 0 (kernel level), and has the lower 5 bits
*  set to the required '14', which is represented by 'E' in
*  hex. */
void isrs_install()
{
    idt_set_gate(0, (unsigned)isr0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned)isr1, 0x08, 0x8E);
    idt_set_gate(2, (unsigned)isr2, 0x08, 0x8E);
    idt_set_gate(3, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(4, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(5, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(6, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(7, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(8, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(9, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(10, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(11, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(12, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(13, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(14, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(15, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(16, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(17, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(18, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(19, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(20, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(21, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(22, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(23, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(24, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(25, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(26, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(27, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(28, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(29, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(30, (unsigned)isr30, 0x08, 0x8E);
    idt_set_gate(31, (unsigned)isr31, 0x08, 0x8E);
}

/* Prints the current state of the registers to the screen in Hex
* format. */
void print_registers(struct regs *r)
{
	char reg[8];
	puts("EIP: ");
	int2hex(r->eip, reg);
	puts(reg);
	
	puts("   CS:  ");
	int2hex(r->cs, reg);
	puts(reg);
	
	puts("   EFLAGS: ");
	int2hex(r->eflags, reg);
	puts(reg);
	
	puts("   U-ESP: ");
	int2hex(r->useresp, reg);
	puts(reg);
	
	puts("\nSS:  ");
	int2hex(r->ss, reg);
	puts(reg);
	
	puts("   EDI: ");
	int2hex(r->edi, reg);
	puts(reg);
	
	puts("   ESI:    ");
	int2hex(r->esi, reg);
	puts(reg);
	
	puts("   EBP:   ");
	int2hex(r->ebp, reg);
	puts(reg);
	
	puts("\nEBX: ");
	int2hex(r->ebx, reg);
	puts(reg);
	
	puts("   EDX: ");
	int2hex(r->edx, reg);
	puts(reg);
	
	puts("   ECX:    ");
	int2hex(r->ecx, reg);
	puts(reg);
	
	puts("   EAX:   ");
	int2hex(r->eax, reg);
	puts(reg);
	
	puts("\nGS:  ");
	int2hex(r->gs, reg);
	puts(reg);
	
	puts("   FS:  ");
	int2hex(r->fs, reg);
	puts(reg);
	
	puts("   ES:     ");
	int2hex(r->es, reg);
	puts(reg);
	
	puts("   DS:    ");
	int2hex(r->ds, reg);
	puts(reg);
}

/* All of our Exception handling Interrupt Service Routines will
*  point to this function. This will tell us what exception has
*  happened! Right now, we simply halt the system by hitting an
*  endless loop. All ISRs disable interrupts while they are being
*  serviced as a 'locking' mechanism to prevent an IRQ from
*  happening and messing up kernel data structures */
void fault_handler(struct regs *r)
{
    /* Is this a fault whose number is from 0 to 31? */
    if (r->int_no < 32)
    {
        /* Display the description for the Exception that occurred.
        *  In this tutorial, we will simply halt the system using an
        *  infinite loop */
	settextcolor(0xFF, 0x01);
        cls();
	puts("Exception: ");
        puts(exception_messages[r->int_no]);
	putch('\n');
	print_registers(r);
	puts("\nSystem Halted!\n");
        for (;;);
    }
}
