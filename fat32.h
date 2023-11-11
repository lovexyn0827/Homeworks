#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Some invalid members are not stripped off.
typedef struct {
	uint8_t jmpBootOffset;
	uint8_t OEMName[8];	// No tailing byte reserved
	uint16_t BytesPerSec;
	uint8_t SecPerClus;
	uint16_t RsvdSecCnt;
	uint8_t NumFATs;
	uint16_t RootEntCnt;	// Must be 0
	uint16_t TotSec16;	// Must be 0
	uint8_t Media;
	uint16_t FATSz16;	// Must be 0
	uint16_t SecPerTrk;
	uint16_t NumHeads;
	uint32_t HiddSec;
	uint32_t TotSec32;	// Must NOT be 0
	uint32_t FATSz32;
	uint16_t ExtFlags;
	uint16_t FSVer;	// Must be 0x0
	uint32_t RootClus;	// Should be 2 or the first usable (not bad) cluster available thereafter.
	uint16_t FSInfo;	// Usually 6
	uint16_t BkBootSec;	// Set to 0 or 6
	uint8_t Reserved[12];	// Must be 0x0
	uint8_t DrvNum;	// Set to 0x0 or 0x80
	uint8_t Reserved1[1];	// Set to 0
	uint8_t BootSig;	// Set value to 0x29 if either of the following two fields are non - zero.
	uint32_t VolID;
	uint8_t VolLab[11];	// No tailing byte reserved
	uint8_t FilSysType[8];	// Set to the string:"FAT32 ".
	// 420 bytes of 0x00 and 0x55 0xAA
} PBR;

#define EXT_FLAG_ACTIVE_FAT_MASK 0x7
#define EXT_FLAG_SINGLE_ACTIVE_FAT 0x40

#define FAT_VALUE_FREE 0x0000000U
#define FAT_VALUE_BAD 0xFFFFFF7U
#define FAT_VALUE_EOF 0xFFFFFFFFU
#define FAT_VALUE_EOF_LOOSE_MIN 0xFFFFFF8
#define FAT_VALUE_EOF_LOOSE_MAX 0xFFFFFFE

uint8_t activeFATNum;

typedef struct {
	// uint32_t LeadSig;	// Value = 0x41615252
	// 480 bytes of 0x00
	// uint32_t StrucSig;	// Value = 0x61417272
	uint32_t Free_Count;
	uint32_t Next_Free;
	// 12 bytes of 0x00
	// uint32_t TrailSig;	// Value = 0xAA550000
} FSInfo;

typedef struct {
	uint8_t Name[11];	// Short name 8 + 3
	uint8_t Attr;
	uint8_t NTRes;	// Must be 0x0
	uint8_t CrtTimeTenth; // 0 <= CrtTimeTenth <= 199
	uint16_t CrtTime;
	uint16_t CrtDate;
	uint16_t LstAccDate;
	uint16_t FstClusHI;
	uint16_t WrtTime;
	uint16_t WrtDate;
	uint16_t FstClusLO;
	uint32_t FileSize;
} DirEntry;

#define KANJI_CHARSET_NAME_FLAG_HANDLING false

#define FILE_NAME_UTF8 true

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20
#define ATTR_LONG_NAME (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

#define NULL_CLUSTER -1

#define DIR_ENTRY_SIZE 32	// We cannot use sizeof(...) as a consequence of the disturbing padding

//#define UNCERTAIN_FILE_SIZE -1	// Enables special handling. The reading ends at the end of the last cluster

#define STEP_CLUSTER_SUCCESS 0
#define STEP_CLUSTER_EOF 1
#define STEP_CLUSTER_ERROR 2

typedef unsigned short unichar;

const uint8_t ROOT_DIR_NAME[11];
const unichar ROOT_DIR_NAME_UNICODE[2];

typedef struct {
	uint8_t Ord;
	unichar Name1[5];	// 10 Bytes
	uint8_t Attr;
	uint8_t Type;	// Must be 0x0
	uint8_t Chksum;
	unichar Name2[6];	// 12 Bytes
	uint16_t FstClusLO;	// Must be 0x0
	unichar Name3[2];	// 4 Bytes
} LNameDirEntry;

/* Usually a file*/
typedef struct {
	DirEntry* head;
	uint8_t entryCount;
	LNameDirEntry* entries;	// For files without any long name entries, this field is null.
	unichar* name;
} LNameDirEntryGroup;

/* A directory*/
typedef struct {
	LNameDirEntryGroup* self;
	uint32_t fileCount;
	LNameDirEntryGroup* files;
} Dir;

typedef struct {
	PBR* PBR;
	FSInfo* fsInfo;
	Dir* rootDir;
	uint64_t freeSpace;
	uint32_t badClusterCount;
	uint32_t* badClusters;
} FAT32Partition;

typedef struct {
	FILE* fp;	// Currently we make no assumption on its position.
	PBR* context;
	uint32_t totalBytes;
	uint32_t cluster;
	uint32_t offsetInCluster;
	uint32_t prevCluster;	// We are going to do some magic somewhere...
	uint32_t clusterOrdinal;
	uint32_t beginningCluster;	// Shouldn't be modified after initialization
	bool isAtDetectedBadCluster;
	bool fileSizeUnknown;
} FAT32FilePointer;

PBR* readPBR(FILE* fp);

FSInfo* readFSInfo(FILE* fp);

DirEntry* readDirEntryToBuf(FAT32FilePointer* ffp, DirEntry* result);

DirEntry* readDirEntry(FAT32FilePointer* ffp);

LNameDirEntry* readLNameDirEntryToBuf(FAT32FilePointer* ffp, LNameDirEntry* result);

LNameDirEntry* readLNameDirEntry(FAT32FilePointer* ffp);

/* Parse the short name of a DirEntry*/
void parseShortNameToBuf(DirEntry* dirPtr, unsigned char* buf);

/* Parse the short name of a DirEntry*/
char* parseShortName(DirEntry* dirPtr);

inline uint32_t getClusterSize(PBR* PBR);

inline uint64_t getClusterOffset(PBR* PBR, uint32_t cluster);

uint32_t estimateFileSize(FILE* fp, PBR* PBR, uint32_t startingCluster);

FAT32FilePointer* newFAT32Fp(FILE* fp, LNameDirEntryGroup* file, PBR* context, bool unknownSize);

inline uint32_t ftellFAT32(FAT32FilePointer* ffp);

inline bool canReadWhenOffseted(FAT32FilePointer* ffp, int64_t offset);

inline bool canReadAt(FAT32FilePointer* ffp, int64_t pos);

inline void resyncCstdioFpOfFFP(FAT32FilePointer* ffp);

/*
  Steps to the next cluster.
  Returns true if the file has the next cluster, or false is EOF or a bad cluster is encountered.
  The pointer remains untouched if the function returns false.
*/
uint8_t stepToTheNextCluster(FAT32FilePointer* ffp);

/*
  Moving the pointer backwards may be much more expensive than moving forward.
  We don't use int32_t as the range of its values may be insufficient.
  Returns false when ffp is going to be moved out of the file or any error has occurred.
*/
bool fseekFAT32(FAT32FilePointer* ffp, int64_t offset, int mode, bool ignoreErrors);

/*
  But anyway, we would usually expect ffp->fp to be at the next byte of what we read after execution.
  The pointer stoppes at the end of the last readable cluster if any error is encountered,
  but I have to say the behavior of the function is undefined after ignoring errors;
*/
uint32_t freadFAT32(void* buf, uint32_t elementSize, uint32_t elementNum, FAT32FilePointer* ffp, bool ignoreErrors);

unsigned char* encodeUtf8(unichar* in);

/*
  Initially, the file pointer should point to the first byte of an entry.
  And when shortNameEntry is true, it is up to us to make sure that the entry is a short name entry.
*/
LNameDirEntryGroup* readLNameDirEntryGroup(FAT32FilePointer* ffp, bool shortNameEntry);

Dir* readDir(FILE* fp, LNameDirEntryGroup* dirFile, PBR* PBR);

Dir* readRootDir(FILE* fp, PBR* PBR);

inline fpos_t getFATLocation(uint32_t cluster, PBR* PBR);

inline fpos_t getFATBeginLocation(PBR* PBR);

uint64_t calculateFreeSpaceUncached(FILE* fp, PBR* PBR);

FAT32Partition* readPartition(FILE* fp);