#ifndef FAT32_H_
#define FAT32_H_

typedef struct fat32Hdd fat32Hdd;
typedef struct fat32File fat32File;
typedef struct fat32Info fat32Info;
typedef struct fat32PartitionTable fat32PartitionTable;

struct fat32Info
{
	char OEMName[8];
	short int BytesPerSector;
	char SectorsPerCluster;
	short int ReservedSectors;
	char FATs;
	short int MaxRootDirs;
	short int TotalSectors;
	char MediaDescriptor;
	short SectorsPerFAT;
	short SectorsPerTrack;
	short Heads;
	int HiddenSectors;
	int TotalSectorsLarge;
	int SectorsPerFAT32;
	short FATFlags;
	short Version;
	int RootDirCluster;
	short FSInfoSector;
	short BootCopySector;
	int Reserved1[3];
	char PhysicalDriveNumber;
	char Reserved2;
	char Signature;
	int ID;
	char VolumeLabel[11];
	char FatType[8];
} __attribute__((packed));

struct fat32PartitionTable
{
	char BootFlag;
	char CHSBegin[3];
	char TypeCode;
	char CHSEnd[3];
	long LBABegin;
	int Sectors;
	
} __attribute__((packed));

struct fat32File
{
	int WeirdStuff[8];
	char ShortFileName[11];
	char Attr;
	int Reserved[2];
	short ClusterHigh;
	int Reserved2;
	short ClusterLow;
	int Size;
	
} __attribute__((packed));

struct fat32Hdd
{
	int Device;
	fat32Info* FatInfo;
	fat32PartitionTable* PartitionTableInfo;
	
};

extern int fat32ParseBootSector(void* sectorStart, fat32Info* info);
extern int fat32ParsePartitionTable(int entry, void* sectorStart, fat32PartitionTable* info);
extern int fat32ParseFileInfo(void* fileStart, fat32File* info);
extern long fat32GetSizeCluster(long* fatTable, long cluster);
extern long fat32GetNextCluster(long* fatTable, long cluster);
extern int fat32Mount(int device);
extern int fat32ReadRootDir(int device, long* rootDirPnt);
extern int fat32ListRootDir(int device);
extern int fat32ReadFile(int device, char* filename, void* buffer, int size);

extern void fat32DebugPrintFatInfo(fat32Info* info);
extern void DebugPrintPartitionTableInfo(fat32PartitionTable* pInfo);
extern void DebugPrintFileInfo(fat32File* fInfo);

#endif
