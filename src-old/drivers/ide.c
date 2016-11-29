#include "../kernel/include/system.h"
#include "../kernel/include/kernlib.h"
#include "ide.h"

typedef struct {
    short num_Cylinders;
    short num_Heads;
    short num_SectorsPerTrack;
    short num_BytesPerSector;
} ideDisk;

int ideDebug = 0;
int numDrives = 1;
static ideDisk drives[2];

static int readDriveConfig(int drive)
{
	int i;
	int status;
	short info[256];
	long long int bytes;

	outportb(IDE_DRIVE_HEAD_REGISTER, (drive == 0) ? IDE_DRIVE_0 : IDE_DRIVE_1);
	outportb(IDE_COMMAND_REGISTER, IDE_COMMAND_IDENTIFY_DRIVE);
	while (inportb(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);

	status = inportb(IDE_STATUS_REGISTER);
	if ((status & IDE_STATUS_DRIVE_DATA_REQUEST)) {
		puts("IDE: Probe found ATA drive:\n");
         	// drive responded to ATA probe
		for (i=0; i < 256; i++) {
			info[i] = inportw(IDE_DATA_REGISTER);
		}

		drives[drive].num_Cylinders = info[IDE_INDENTIFY_NUM_CYLINDERS];
		drives[drive].num_Heads = info[IDE_INDENTIFY_NUM_HEADS];
		drives[drive].num_SectorsPerTrack = info[IDE_INDENTIFY_NUM_SECTORS_TRACK];
		drives[drive].num_BytesPerSector = info[IDE_INDENTIFY_NUM_BYTES_SECTOR];
	} else {
       		// try for ATAPI
		outportb(IDE_FEATURE_REG, 0);		// disable dma & overlap

		outportb(IDE_DRIVE_HEAD_REGISTER, (drive == 0) ? IDE_DRIVE_0 : IDE_DRIVE_1);
		outportb(IDE_COMMAND_REGISTER, IDE_COMMAND_ATAPI_IDENT_DRIVE);
		while (inportb(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);
		status = inportb(IDE_STATUS_REGISTER);
		puts("IDE: Found atapi drive\n");
		return -1;
	}

	kprintf("IDE:     %d cylinders, %d heads, %d sectors/track, %d bytes/sector\n", 
	       drives[drive].num_Cylinders, drives[drive].num_Heads,
	       drives[drive].num_SectorsPerTrack, drives[drive].num_BytesPerSector);
	bytes = ((long long int )IDE_getNumBlocks(drive)) * 512;
	kprintf("IDE:     Disk has %d blocks (%u bytes)\n", IDE_getNumBlocks(drive), bytes);

	return 0;
}

void Init_IDE()
{
	int ret;
	int errorCode;

	#ifndef BOCHS 
	while (inportb(IDE_STATUS_REGISTER) != 0x50); // This freezes BOCHS emu
	#endif
	
	outportb(IDE_COMMAND_REGISTER, IDE_COMMAND_DIAGNOSTIC);
	while (inportb(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);
	errorCode = inportb(IDE_ERROR_REGISTER);

	ret = readDriveConfig(0);
	if (ret < 0) {
		numDrives = 0;
		return;
	} else if (errorCode & 0x80) {
		puts("IDE: Found one drive\n");
		numDrives = 1;
	} else {
		puts("IDE: Found second drive\n");
		numDrives = 2;
		readDriveConfig(1);
	}
}

int IDE_getNumBlocks(int driveNum)
{
	if (driveNum < 0 || driveNum > (numDrives-1)) {
		return IDE_ERROR_BAD_DRIVE;
	}

	return (drives[driveNum].num_Heads * 
			drives[driveNum].num_SectorsPerTrack *
			drives[driveNum].num_Cylinders);
}

int IDE_Read(int driveNum, int blockNum, char *buffer)
{
	int i;
	int head;
	int sector;
	int cylinder;
	short *bufferW;
	int reEnable = 0;

	if (driveNum < 0 || driveNum > (numDrives-1)) {
		puts("IDE: invalid drive \n");
		return IDE_ERROR_BAD_DRIVE;
	}

	if (blockNum < 0 || blockNum >= IDE_getNumBlocks(driveNum)) {
		puts("IDE: invalid block\n");
		return IDE_ERROR_INVALID_BLOCK;
	}

	if (interrupts_enabled()) {
		disable_interrupts();
		reEnable = 1;
	}

	/* now compute the head, cylinder, and sector */
	sector = blockNum % drives[driveNum].num_SectorsPerTrack + 1;
	cylinder = blockNum / (drives[driveNum].num_Heads * 
			drives[driveNum].num_SectorsPerTrack);
	head = (blockNum / drives[driveNum].num_SectorsPerTrack) % 
			drives[driveNum].num_Heads;

	if (ideDebug) {
		kprintf("IDE: Request to read block %d\n", blockNum);
		kprintf("IDE:     head %d\n", head);
		kprintf("IDE:     cylinder %d\n", cylinder);
		kprintf("IDE:     sector %d\n", sector);
	}

	outportb(IDE_SECTOR_COUNT_REGISTER, 1);
	outportb(IDE_SECTOR_NUMBER_REGISTER, sector);
	outportb(IDE_CYLINDER_LOW_REGISTER, LOW_BYTE(cylinder));
	outportb(IDE_CYLINDER_HIGH_REGISTER, HIGH_BYTE(cylinder));
	if (driveNum == 0) {
		outportb(IDE_DRIVE_HEAD_REGISTER, IDE_DRIVE_0 | head);
	} else if (driveNum == 1) {
		outportb(IDE_DRIVE_HEAD_REGISTER, IDE_DRIVE_1 | head);
	}

	outportb(IDE_COMMAND_REGISTER, IDE_COMMAND_READ_SECTORS);

	if (ideDebug) {puts("IDE: Waiting for read... \n");}

	/* wait for the drive */
	while (inportb(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);

	if (inportb(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_ERROR) {
		kprintf("IDE: <ERROR> Got Read %d\n", inportb(IDE_STATUS_REGISTER));
		return IDE_ERROR_DRIVE_ERROR;
	}

	
	if (ideDebug) { kprintf("IDE: Reading to 0x%x\n", buffer); }

	bufferW = (short *) buffer;
	for (i=0; i < 256; i++) {
		bufferW[i] = inportw(IDE_DATA_REGISTER);
	}

	if (reEnable) enable_interrupts();

	return IDE_ERROR_NO_ERROR;
}

int IDE_Write(int driveNum, int blockNum, char *buffer)
{
	int i;
	int head;
	int sector;
	int cylinder;
	short *bufferW;
	int reEnable = 0;

	if (driveNum < 0 || driveNum > (numDrives-1)) {
		return IDE_ERROR_BAD_DRIVE;
	}

	if (blockNum < 0 || blockNum >= IDE_getNumBlocks(driveNum)) {
		return IDE_ERROR_INVALID_BLOCK;
	}

	if (interrupts_enabled()) {
		disable_interrupts();
		reEnable = 1;
	}

	/* now compute the head, cylinder, and sector */
	sector = blockNum % drives[driveNum].num_SectorsPerTrack + 1;
	cylinder = blockNum / (drives[driveNum].num_Heads * 
			drives[driveNum].num_SectorsPerTrack);
	head = (blockNum / drives[driveNum].num_SectorsPerTrack) % 
			drives[driveNum].num_Heads;

	if (ideDebug) {
	kprintf("IDE: Request to write block %d\n", blockNum);
	kprintf("IDE:     head %d\n", head);
	kprintf("IDE:     cylinder %d\n", cylinder);
	kprintf("IDE:     sector %d\n", sector);
	}

	outportb(IDE_SECTOR_COUNT_REGISTER, 1);
	outportb(IDE_SECTOR_NUMBER_REGISTER, sector);
	outportb(IDE_CYLINDER_LOW_REGISTER, LOW_BYTE(cylinder));
	outportb(IDE_CYLINDER_HIGH_REGISTER, HIGH_BYTE(cylinder));
	if (driveNum == 0) {
		outportb(IDE_DRIVE_HEAD_REGISTER, IDE_DRIVE_0 | head);
	} else if (driveNum == 1) {
		outportb(IDE_DRIVE_HEAD_REGISTER, IDE_DRIVE_1 | head);
	}

	outportb(IDE_COMMAND_REGISTER, IDE_COMMAND_WRITE_SECTORS);


	/* wait for the drive */
	while (inportb(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);

	bufferW = (short *) buffer;
	for (i=0; i < 256; i++) {
		outportw(IDE_DATA_REGISTER, bufferW[i]);
	}

	if (ideDebug) 
		puts("IDE: About to wait for Write \n");

	/* wait for the drive */
	while (inportb(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);

	if (inportb(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_ERROR) {
		kprintf("IDE: <ERROR> Got Read %d\n", inportb(IDE_STATUS_REGISTER));
		return IDE_ERROR_DRIVE_ERROR;
	}

	if (reEnable) enable_interrupts();
	
	return IDE_ERROR_NO_ERROR;
}

void IDE_SetDebugging(int debug)
{
	ideDebug = debug;
	puts("IDE: Debugging enabled...\n");
}
