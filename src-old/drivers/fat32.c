#include <kernlib.h>
#include <system.h>
#include <drivers.h>
#include "fat32.h"

fat32Hdd* HddStructs[4];
long* fat32FatTable[4];

int fat32ParseBootSector(void* sectorStart, fat32Info* info)
{
	memcpy(info, sectorStart + sizeof(char[3]), sizeof(fat32Info));
	return 1;
}

int fat32ParsePartitionTable(int entry, void* sectorStart, fat32PartitionTable* info)
{
	memcpy(info, sectorStart + 0x1be + (sizeof(fat32Info) * entry), sizeof(fat32Info));
	return 1;
}

int fat32ParseFileInfo(void* fileStart, fat32File* info)
{
	memcpy(info, fileStart, sizeof(fat32File));
	return 1;
}

long fat32GetNextCluster(long* fatTable, long cluster)
{
	return fatTable[cluster];
}

long fat32GetSizeCluster(long* fatTable, long startCluster)
{
	long count = 1;
	while (((startCluster = fat32GetNextCluster(fatTable, startCluster)) & 0x0FFFFFFF) < 0x0FFFFFF8 )
		count++;
	return count;
}

int fat32ReadFAT(int device, long* fatTable)
{
	//Read FAT Table
	long fatSectors = HddStructs[device]->FatInfo->SectorsPerFAT;
	void* table = (void*) malloc(fatSectors * HddStructs[device]->FatInfo->BytesPerSector);
	
	long startSector = HddStructs[device]->FatInfo->ReservedSectors;
	long* fatPnt = (long*) table;
	
	// If the bootsector was messed up the number of sectors for the FAT is corrupted too
	if (fatSectors < 1)
	{
		kprintf("Error: Can't read %d numbers of sectors.", device);
		return 0;
	}
		
	// Read in all the sectors of the root dir.
	while (fatSectors > 0)
	{
		IDE_Read(device, startSector++, (void*) fatPnt);
		fatPnt += HddStructs[device]->FatInfo->BytesPerSector;
		fatSectors--;
	}
	*fatTable = (long) table;
	return 1;
}

int fat32Mount(int device)
{
	// Read in bootsector
	char* bootSector = (char*) malloc(512);	
	IDE_Read(device, 0, bootSector);
	
	//check MBR
	if ((*(bootSector + 0x1FE) & 0x55) != 0x55)
	{
		kprintf("Error mounting device: %d (MBR Corrupted)\n", device);
		return -1;
	}
	
	fat32Hdd* hddStr = (fat32Hdd*) malloc(sizeof(fat32Hdd));
		
	// Retrieve bootsector info
	fat32Info* info = (fat32Info*) malloc(sizeof(fat32Info));
	fat32ParseBootSector(bootSector, info);
	hddStr->FatInfo = info;
	
	// Check if it actually is FAT32 before we start messing around
	if (strstr(info->FatType, "FAT32 ") == NULL)
	{
		kprintf("Error mounting device: %d (No FAT Filesystem found)\n", device);
		free (hddStr);
		free (info);
		free (bootSector);
		return -1;
	}
		
	// Retrieve Partition Table info
	fat32PartitionTable* pInfo = (fat32PartitionTable*) malloc(sizeof(fat32PartitionTable));
	fat32ParsePartitionTable(0, bootSector, pInfo);
	hddStr->PartitionTableInfo = pInfo;
	hddStr->Device = device;
	
	HddStructs[device] = hddStr;
	
	// Cache the FAT
	if (fat32ReadFAT(device, (long*)(&fat32FatTable[device])) != 1)
	{
		kprintf("Error listning Root Dir: Can not read File Allocation Table.\n", device);
		free (hddStr);
		free (info);
		free (bootSector);
		return 0;
	}

	free (bootSector);
	kprintf("Device: %d, FS: FAT32, BPS: %d, SPC: %d\n", device, info->BytesPerSector, info->SectorsPerCluster);
	
	return 1;
}

int fat32ReadRootDir(int device, long* rootDirPnt)
{
	long* fatTable =  fat32FatTable[device];
	
	// Check if the device is mounted
	if (HddStructs[device] == 0)
	{
		kprintf("Error listning Root Dir: Device %d not mounted\n", device);
		return 0;
	}
	
	// Check if we need to read more clusters
	long nClus = fat32GetNextCluster(fatTable, HddStructs[device]->FatInfo->RootDirCluster);
	if ((nClus & 0x0FFFFFFF) < 0x0FFFFFF8 )
	{
		// We need to read more clusters to get the root entry, first calc how many
		nClus = fat32GetSizeCluster(fatTable, HddStructs[device]->FatInfo->RootDirCluster);
		
		// Reserve space
		void* rootdirClus = (void*) malloc(HddStructs[device]->FatInfo->SectorsPerCluster * HddStructs[device]->FatInfo->BytesPerSector * nClus);
		long rootDir = HddStructs[device]->FatInfo->ReservedSectors + (HddStructs[device]->FatInfo->FATs * HddStructs[device]->FatInfo->SectorsPerFAT32);
		
		while (nClus > 0)
		{
			IDE_Read(0, rootDir++, rootdirClus);
			nClus--;
		}
		*rootDirPnt = (long) rootdirClus;
	}
	else
	{
		// Just read in first cluster
		void* rootdirClus = (void*) malloc(HddStructs[device]->FatInfo->SectorsPerCluster * HddStructs[device]->FatInfo->BytesPerSector);
		long rootDir = HddStructs[device]->FatInfo->ReservedSectors + (HddStructs[device]->FatInfo->FATs * HddStructs[device]->FatInfo->SectorsPerFAT32);
		IDE_Read(device, rootDir, rootdirClus);
		*rootDirPnt = (long) rootdirClus;
}
	
	free (fatTable);
	return 1;
}

int fat32ListRootDir(int device)
{
	long rootDir = 0;
	if(fat32ReadRootDir(device, &rootDir) != 1)
	{
		kprintf("Error reading Root Dir from device %d\n", device);
		return 0;
	}
	
	fat32File* file = (fat32File*)rootDir;
	while (*(long*)file != 0)
	{
		// for now just print to screen
		kprintf("%s %d Byte\n", file->ShortFileName, file->Size);
		file++;
	}
	free ( (long*)rootDir);
	return 1;
}

int fat32ReadFile(int device, char* filename, void* buffer, int size)
{
	long rootDir = 0;
	if(fat32ReadRootDir(device, &rootDir) != 1)
	{
		kprintf("Error reading Root Dir from device %d\n", device);
		return 0;
	}
	
	fat32File* file = (fat32File*)rootDir;
	while (*(long*)file != 0)
	{
		if (strstr(file->ShortFileName, filename))
		{
			long cluster = ((file->ClusterHigh << 16) + file->ClusterLow);
			kprintf("File found, cluster: %d\n", cluster);
			
			while (size > 0)
			{
				IDE_Read(device, ((HddStructs[device]->FatInfo->ReservedSectors + (HddStructs[device]->FatInfo->FATs * HddStructs[device]->FatInfo->SectorsPerFAT32) + cluster) * HddStructs[device]->FatInfo->SectorsPerCluster) - HddStructs[device]->FatInfo->RootDirCluster, (char*) buffer);
				fat32GetNextCluster(fat32FatTable[device], cluster);
				size -= 512;
			}
		}
		file++;
	}
	free ( (long*)rootDir);
	return 1;
}

void fat32DebugPrintFatInfo(fat32Info* info)
{
	// FAT info
	kprintf(" OEM Name: %s\n", info->OEMName);
	kprintf(" Bytes Per Sector: %d\n", info->BytesPerSector);
	kprintf(" Sectors Per Cluster: %d\n", info->SectorsPerCluster);
	kprintf(" Reserved Sectors: %d\n", info->ReservedSectors);
	kprintf(" FATs: %d\n", info->FATs);
	kprintf(" Max Root Dirs: %d\n", info->MaxRootDirs);
	kprintf(" Total Sectors: %d\n", info->TotalSectors);
	kprintf(" Media Descriptor: %x\n", info->MediaDescriptor);
	kprintf(" Sectors Per Fat: %d\n", info->SectorsPerFAT);
	kprintf(" Sectors Per Track: %d\n", info->SectorsPerTrack);
	kprintf(" Number of Heads: %d\n", info->Heads);
	kprintf(" Hidden Sectors: %d\n", info->HiddenSectors);
	kprintf(" Total Sectors (large): %d\n", info->TotalSectorsLarge);
	
	// Fat32 Part
	kprintf(" Sectors Per Fat (32): %d\n", info->SectorsPerFAT32);
	kprintf(" Fat Flags: %x\n", info->FATFlags);
	kprintf(" Version: %d\n", info->Version);
	kprintf(" Root dir cluster: %d\n", info->RootDirCluster);
	kprintf(" Info Sector: %d\n", info->FSInfoSector);
	kprintf(" Boot Copy Sector: %x\n", info->BootCopySector);
	kprintf(" Drive number: %d\n", info->PhysicalDriveNumber);
	kprintf(" Signature: %x\n", info->Signature);
	kprintf(" ID: %x\n", info->ID);
	kprintf(" Volume Label: %s\n", info->VolumeLabel);
	kprintf(" Fat Type: %s\n", info->FatType);
		
}

void DebugPrintPartitionTableInfo(fat32PartitionTable* pInfo)
{
	// Partition Table
	kprintf("Partition 0 Info:\n");
	kprintf(" Type Code: %x\n", pInfo->TypeCode);
	kprintf(" LBA Begin: %x\n", pInfo->LBABegin);
	kprintf(" Sectors: %d\n", pInfo->Sectors);	
}

void DebugPrintFileInfo(fat32File* fInfo)
{
	kprintf(" Filename: %s\n", fInfo->ShortFileName);
	kprintf(" Attribute: %x\n", fInfo->Attr);
	kprintf(" Cluster High: %x\n", fInfo->ClusterHigh);
	kprintf(" Cluster Low: %x\n", fInfo->ClusterLow);
	kprintf(" Size: %d\n", fInfo->Size);	
}
