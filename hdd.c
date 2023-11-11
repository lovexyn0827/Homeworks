#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "hdd.h"

#define PARTITION_EXPORT_BUFFER_SIZE 16777216

void readCHSToBuf(FILE* fp, CHSAddr* buf) {
	fread(&buf->head, 1, 1, fp);
	uint8_t bytes[2];
	fread(bytes, 1, 2, fp);
	buf->sector = bytes[0] >> 2;
	buf->cylinder = bytes[0] & 0x3 << 8 | bytes[1];
}

void readPartitionToBuf(FILE* fp, Partition* part, bool logical) {
	fread(&part->status, 1, 1, fp);
	readCHSToBuf(fp, &part->startCHS);
	fread(&part->fsCode, 1, 1, fp);
	readCHSToBuf(fp, &part->endCHS);
	fread(&part->startLBA, 4, 1, fp);
	fread(&part->sectorCount, 4, 1, fp);
	part->isLogical = logical;
}

void readPartitionTableToBuf(FILE* fp, PartitionTable* pt, bool includeLogical) {
	Partition* partitions = (Partition*)malloc(256 * sizeof(Partition));
	if (partitions == NULL) {
		return;
	}

	int partitionCount = 0;
	for (int i = 0; i < 4; i++) {
		fseek(fp, PARTITION_TABLE_OFFSET + PARTITION_ENTRY_SIZE * i, SEEK_SET);
		Partition* current = partitions + partitionCount++;
		readPartitionToBuf(fp, current, true);
		if (!current->startLBA) {
			partitionCount--;
			continue;
		}

		// Haven't been tested
		if (includeLogical) {
			while (true) {
				uint64_t nextPartEntryOffset = (uint64_t)(current->startLBA) * SECTOR_SIZE + NEXT_LOGICAL_PARTITION_OFFSET;
				fsetpos(fp, &nextPartEntryOffset);
				Partition mayValid;
				readPartitionToBuf(fp, &mayValid, true);
				if (mayValid.startLBA) {
					current = &mayValid;
					memcpy(partitions + partitionCount++, current, sizeof(Partition));
				} else {
					break;
				}
			}
		}
	}

	pt->partitionCount = partitionCount;
	pt->partitions = partitions;
}

void extractPartition(FILE* disk, Partition* part, char* output) {
	uint64_t start = (uint64_t)part->startLBA * SECTOR_SIZE;
	FILE* volWriteable = fopen(output, "wb");
	void* buf = malloc(PARTITION_EXPORT_BUFFER_SIZE);
	if (buf == NULL || volWriteable == NULL) {
		return;
	}

	fsetpos(disk, &start);
	while (true) {
		uint64_t readBytes = fread(buf, 1, PARTITION_EXPORT_BUFFER_SIZE, disk);
		uint64_t written = fwrite(buf, 1, readBytes, volWriteable);
		if (readBytes < PARTITION_EXPORT_BUFFER_SIZE) {
			break;
		}
	}

	free(buf);
}