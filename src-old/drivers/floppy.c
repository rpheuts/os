#include "../kernel/include/system.h"
#include "../kernel/include/kernlib.h"
#include "floppy.h"

void FLP_detect()
{
	unsigned char c;
	outportb(0x70, 0x10);
	c = inportb(0x71);
	
	int a = c >> 4; // get the high nibble
	int b = c & 0xF; // get the low nibble by ANDing out the high nibble
	
	char *drive_type[6] = { "no floppy drive", "360kb 5.25in floppy drive", "1.2mb 5.25in floppy drive", "720kb 3.5in", "1.44mb 3.5in", "2.88mb 3.5in"};
	kprintf("FLP: Floppy drive A: %s\n", drive_type[a]);
	kprintf("FLP: Floppy drive B: %s\n", drive_type[b]);
}
