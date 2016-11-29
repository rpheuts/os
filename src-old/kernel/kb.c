#include <system.h>

void (*AppHandle)(unsigned char, int);

/* Handles the keyboard interrupt */
void keyboard_handler(struct regs *r)
{
	unsigned char scancode;

	/* Read from the keyboard's data buffer */
	scancode = inportb(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
	if (scancode & 0x80)
	{
		if (AppHandle)
			AppHandle(scancode, 1);
	}
	else
	{
		if (AppHandle)
			AppHandle(scancode, 0);
	}
}

void keyboard_handler_set(void(*handler)(unsigned char, int))
{
	AppHandle = handler;
}

/* Registers IRQ 1 to the keyboard handler function */
void keyboard_install()
{
	/* Installs 'keyboard_handler' to IRQ0 */
	irq_install_handler(1, keyboard_handler);
}
