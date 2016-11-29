#include <system.h>
#include <kernlib.h>
#include <drivers.h>

/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */
unsigned char kbdus[144] =
{
	0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
	'9', '0', '-', '=', '\b',	/* Backspace */
	'\t',			/* Tab */
	'q', 'w', 'e', 'r',	/* 19 */
	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
	0,			/* 29   - Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
	'\'', '`',   0,		/* Left shift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
	'm', ',', '.', '/',   0,				/* Right shift */
	'*',
	0,	/* Alt */
	' ',	/* Space bar */
	0,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
	'-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
	'+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0,   0,   0,
	0,	/* F11 Key */
	0,	/* F12 Key */
	0,	/* 89 All other keys are undefined */
	0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
	'(', ')', '_', '+', '\b',	/* Backspace */
	'\t',			/* Tab */
	'Q', 'W', 'E', 'R',	/* 19 */
	'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
	0,			/* 29   - Control */
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
	'\"', '~',   0,		/* Left shift */
	'|', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
	'M', '<', '>', '?', 
};
char command[256];
char* cmdPnt;
int cmdPos = 0;
int shift;
char* previousCommand;

void clear_input(int tty)
{
	cmdPnt = &command[0];
	while(*cmdPnt != '\0')
	{
		*cmdPnt++ = '\0';
		if (tty)
		{
			putch('\b');
			putch(' ');
			putch('\b');
		}
	}
	cmdPnt = &command[0];
	cmdPos = 0;
}

void PeekMemory(char* address)
{
	int memLoc = hex2int(address);
	puts(" Pointer: 0x");
	puts(address);
			
	char tmp[20];
	memLoc = *((int*) memLoc);
	int2hex(memLoc, &tmp[0]);
	puts("   Data: 0x");
	puts(&tmp[1]);
			
	puts("   Text: ");
	char* ch = (char*) &memLoc;
	putch(ch[0]);
	putch(ch[1]);
	putch(ch[2]);
	putch(ch[3]);
	putch('\n');
}

void PokeMemory(char* address, char* value)
{
	int memLoc = hex2int(address);
	puts(" Pointer: 0x");
	
	*((int*)memLoc) = hex2int(value);
	
	PeekMemory(address);
}

void HandleKey(unsigned char scancode, int released)
{
	if (scancode & 0x80)
	{
		if ((scancode - 0x80) == 54 || (scancode - 0x80) == 42) // Shift
		{
			shift = 0;
		}
	}
	else
	{
        /* Here, a key was just pressed. Please note that if you
		*  hold a key down, you will get repeated key press
	*  interrupts. */
		if (scancode == 54 || scancode == 42) // Shift
		{
			shift = 1;
		}
		else if (scancode == 28) // Enter
		{
			putch('\n');
			HandleCommand(command);
			clear_input(0);
		}
		else if (scancode == 14) // Backspace
		{
			if (cmdPos > 0)
			{
				putch(kbdus[scancode]);
				putch(' ');
				putch(kbdus[scancode]);
				*cmdPnt = 0;
				cmdPnt--;
				cmdPos--;
			}
		}
		else if (scancode == 72) // Up arrow
		{
			clear_input(1);
			char* prevCmd = GetPreviousCommand();
			while (*prevCmd != '\0')
			{
				putch(*prevCmd);
				*cmdPnt++ = *prevCmd++;
				cmdPos++;
			}
		}
		else
		{
			if (shift == 1)
				scancode += 90;
			putch(kbdus[scancode]);
			*cmdPnt++ = kbdus[scancode];
			cmdPos++;
		}
	}
}

void HandleCommand(char* command)
{
	if (strcmp(command, "version") == 0)
	{	
		puts("Blue-Kernel 0.01 Alpha\n");
	}
	else if (strcmp(command, "clear") == 0)
	{	
		cls();
	}
	else if (strstr(command, "peek") != NULL) //peek 0x00000000 0x00000000
	{	
		if (!command[5])
			puts(" No arguments given");
		else if (!command[16])
		{
			char* arg = &command[5];
			PeekMemory(arg);
		}
		else
		{
			command[15] = 0; 
			char* arg = &command[5];
			int start = hex2int(arg);
		
			arg = &command[16];
			int end = hex2int(arg);
			
			char tmp[8];
			
			while (start < end)
			{
				int2hex(start++, tmp);
				PeekMemory(tmp);
			}
			command[15] = ' '; 
		}
		putch('\n');
	}
	else if (strstr(command, "poke") != NULL) //poke 0x00000000 0x00000000
	{
		if (!command[5])
			puts(" No arguments given");
		else if (!command[16])
			puts(" Only one arguments given");
		else if(command[15] != ' ')
			puts(" Argument error");
		else
		{
			command[15] = 0; 
			PokeMemory(&command[5], &command[16]);
			command[15] = ' '; 
		}
		putch('\n');
	}
	else if ((strcmp(command, "help") == 0))
	{
		puts(" Simple Kernel Shell:\n  cls     Clear screen\n  peek    Peek into mem location\n  poke    Poke into mem location\n  version Show version\n  mount <device_num>  Mounts a device\n");
	}
	else if ((strcmp(command, "date") == 0))
	{
		kprintf("%d:%d:%d %d/%d/%d\n", CMOS_GetHours(), CMOS_GetMinutes(), CMOS_GetSeconds(), \
			CMOS_GetDateMonth(), CMOS_GetDateDay(), CMOS_GetDateYear());
	
	}
	else if (strcmp(command, "memdump") == 0)
	{
		puts("Dumping memory:\n");
		memdump();
	}
	else if (strstr(command, "mount") != 0)
	{
		int device = -1;
		if (command[6] == '\0' || str2int(&command[6], &device) != 1)
			kprintf("Invalid device\n");
		else
		{
			fat32Mount(device);
			putch('\n');
		}
	}
	else if (strstr(command, "ls") != 0)
	{
		int device = -1;
		if (command[3] == '\0' || str2int(&command[3], &device) != 1)
			kprintf("Invalid device\n");
		else
		{
			fat32ListRootDir(device);	
			putch('\n');
		}
	}
	else if (strstr(command, "cat") != 0)
	{
		int device = -1;
		char* deviceChr = (char*) malloc(2);
		deviceChr[0] = command[4];
		deviceChr[1] = '\0';
		if (command[4] == '\0' || str2int(deviceChr, &device) != 1)
			kprintf("Invalid device\n");
		else
		{
			void* buffer = malloc(512);
			fat32ReadFile(device, &command[6], buffer, 512);
			
			kprintf("%s\n", (char*) buffer);
			putch('\n');
			free (buffer);
		}
		free(deviceChr);
	}
	else if (strcmp(command, "") == 0)
	{
		// do nothing
	}
	else
	{
		kprintf("\n Unrecognized command: %s\n", command);
	}
	
	// save command
	strncpy(previousCommand, command, sizeof(char[256]));
	
	puts("Kernel # ");
}

char* GetPreviousCommand()
{
	return previousCommand;
}

void Intall_SShell()
{
	previousCommand = (char*) malloc(sizeof(char[256]));
	cmdPnt = &command[0];
	keyboard_handler_set(HandleKey);
	puts("Kernel # ");
}
