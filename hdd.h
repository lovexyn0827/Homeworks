#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define PARTITION_TABLE_OFFSET 0x01BE

#define NEXT_LOGICAL_PARTITION_OFFSET 0x01CE

#define PARTITION_ENTRY_SIZE 16

#define SECTOR_SIZE 512

typedef struct {
	uint8_t head;	// 8 bits
	uint8_t sector;	// 6 bits
	uint16_t cylinder;	// 10 bits
} CHSAddr;

typedef struct {
	uint8_t status;	// Whether the volume is bootable.
	CHSAddr startCHS;
	uint8_t fsCode;
	CHSAddr endCHS;
	uint32_t startLBA;
	uint32_t sectorCount;
	bool isLogical;
} Partition;

typedef struct {
	uint32_t partitionCount;
	Partition* partitions;
} PartitionTable;

void readCHSToBuf(FILE* fp, CHSAddr* buf);

void readPartitionToBuf(FILE* fp, Partition* part, bool logical);

void readPartitionTableToBuf(FILE* fp, PartitionTable* pt, bool includeLogical);

void extractPartition(FILE* fp, Partition* part, char* output);