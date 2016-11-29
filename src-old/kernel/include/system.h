#ifndef __SYSTEM_H
#define __SYSTEM_H

#define BOCHS

#include "types.h"

/*
Memory layout:
0x00400000 : Kernel mem (malloc)
0x00100000 : Kernel
0x00091000 : Page Table
0x00090000 : Page Directory
0x00080000 : Page Usage Info
*/

#define LOC_PAGE_TABLE	0x91000
#define LOC_PAGE_DIR	0x90000
#define LOC_PAGE_INFO	0x80000
#define LOC_KERNEL	0x100000
#define LOC_MALLOC_INFO	0x60000

#define PAGE_SIZE 4096

/* This defines what the stack looks like after an ISR was running */
struct regs
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

/* MAIN.C */
extern unsigned char inportb (unsigned short _port);
extern unsigned short inportw (unsigned short _port);
extern void outportb (unsigned short _port, unsigned char _data);
extern void outportw (unsigned short _port, unsigned short _data);
extern void panic(char* text);
extern void enable_interrupts();
extern void disable_interrupts();
extern int interrupts_enabled();

/* SCREEN.C */
extern void cls();
extern void putch(unsigned char c);
extern void puts(unsigned char *str);
extern void settextcolor(unsigned char forecolor, unsigned char backcolor);
extern void init_video();

/* GDT.C */
extern void gdt_install();
extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);

/* IDT.C */
extern void idt_install();
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);

/* ISRS.C */
extern void isrs_install();

/* IRQ.C */
extern void irq_install();
extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);

/* TIMER.C */
extern void timer_install();

/* KB.C */
extern void keyboard_handler_set(void(*handler)(unsigned char, int));
extern void keyboard_install();

/* MEM.C */
extern void install_paging();
extern void* malloc(uint32_t size);
extern void free(void* pnt);
extern void memdump();

/* SSHELL.C */
extern void HandleCommand(char* command);
extern void Intall_SShell();
extern char* GetPreviousCommand();

#endif
