#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "fat32.h"
#include "hdd.h"

#define EXTRACT_BUFFER_SIZE 16777216

void printCommands() {
	puts("ls: List files & directories.");
	puts("lsshort: List the short name of files & directories.");
	puts("cd <dir>: Enter a directory.");
	puts("extract <file> <dst>: Export a file as an independent file.");
	puts("info <file>: Get the detailed info of a file.");
	puts("usefat <fat>: Use a FAT other than the first one.");
	puts("help: List available commands.");
	puts("exit: Exit from the disk image viewer.\n");
}

void openPartitionImage(char* imageFileName) {
	FILE* fp = fopen(imageFileName, "rb");
	if (fp == NULL) {
		perror("");
		return;
	}

	FAT32Partition* vol = readPartition(fp);
	Dir* root = vol->rootDir;
	Dir* currentDir = root;
	activeFATNum = 0;
	printCommands();
	while (true) {
		char cmd[256] = { 0 };
		putchar('$');
		if (!scanf("%s", cmd)) {
			puts("Unexpected error!");
			continue;
		}

		if (strcmp(cmd, "ls") == 0) {
			for (uint32_t i = 0; i < currentDir->fileCount; i++) {
				LNameDirEntryGroup file = currentDir->files[i];
				uint32_t cluster = file.head->FstClusHI << 16 | file.head->FstClusLO;
				printf("%-6d%#-12X%#-18llX%-12ld%#4X%-16s%-48.48s\n", i, cluster, 
					getClusterOffset(vol->PBR, cluster), file.head->FileSize, file.head->Attr, 
					parseShortName(file.head), file.name);
			}
		} else if (strcmp(cmd, "lsshort") == 0) {
			for (uint32_t i = 0; i < currentDir->fileCount; i++) {
				LNameDirEntryGroup file = currentDir->files[i];
				uint32_t cluster = file.head->FstClusHI << 16 | file.head->FstClusLO;
				printf("%-6d%#-12X%#-18llX%-12ld%#4X%-16.16s\n", i, cluster, 
					getClusterOffset(vol->PBR, cluster), file.head->FileSize, file.head->Attr, 
					parseShortName(file.head));
			}
		} else if (strcmp(cmd, "cd") == 0) {
			char name[514] = { 0 };
			if (!scanf("%s", name)) {
				continue;
			}

			for (uint32_t i = 0; i < currentDir->fileCount; i++) {
				LNameDirEntryGroup file = currentDir->files[i];
				uint32_t cluster = file.head->FstClusHI << 16 | file.head->FstClusLO;
				if (strcmp(parseShortName(file.head), name) == 0) {
					if (file.head->Attr & ATTR_DIRECTORY) {
						currentDir = readDir(fp, &currentDir->files[i], vol->PBR);
					} else {
						puts("Not a directory!");
					}

					break;
				}

				if (i == currentDir->fileCount - 1) {
					puts("Not found!");
				}
			}
		} else if (strcmp(cmd, "extract") == 0) {
			char name[514] = { 0 };
			char* dst = (char*)malloc(1024);
			if (!scanf("%s", name) || dst == NULL || !scanf("%s", dst)) {
				continue;
			}

			for (uint32_t i = 0; i < currentDir->fileCount; i++) {
				LNameDirEntryGroup file = currentDir->files[i];
				uint32_t cluster = file.head->FstClusHI << 16 | file.head->FstClusLO;
				if (strcmp(parseShortName(file.head), name) == 0) {
					if (file.head->Attr & ATTR_DIRECTORY) {
						puts("Cannot export a directory!");
					} else {
						FAT32FilePointer* ffp = newFAT32Fp(fp, &currentDir->files[i], vol->PBR, false);
						void* buffer = malloc(EXTRACT_BUFFER_SIZE);
						if (buffer == NULL) {
							puts("Insufficient memory!");
							break;
						}

						FILE* dstFile = fopen(dst, "wb");
						if (dstFile == NULL) {
							perror("Unable to export");
							break;
						}

						uint32_t remaining = file.head->FileSize;
						while (remaining > 0) {
							uint32_t step = remaining >= EXTRACT_BUFFER_SIZE ? EXTRACT_BUFFER_SIZE : remaining;
							uint32_t read = freadFAT32(buffer, 1, step, ffp, false);
							uint32_t rewritten = fwrite(buffer, 1, step, dstFile);
							remaining -= step;
						}

						fclose(dstFile);
						free(buffer);
					}

					break;
				}

				if (i == currentDir->fileCount - 1) {
					puts("Not found!");
				}
			}

			free(dst);
		} else if (strcmp(cmd, "info") == 0) {
			char name[514] = { 0 };
			if (!scanf("%s", name)) {
				continue;
			}

			for (uint32_t i = 0; i < currentDir->fileCount; i++) {
				LNameDirEntryGroup file = currentDir->files[i];
				uint32_t cluster = file.head->FstClusHI << 16 | file.head->FstClusLO;
				if (strcmp(parseShortName(file.head), name) == 0) {
					char attrStr[10] = "";
					uint8_t attr = file.head->Attr;
					if (attr & ATTR_ARCHIVE) {
						strcat(attrStr, "A");
					}

					if (attr & ATTR_DIRECTORY) {
						strcat(attrStr, "D");
					}

					if (attr & ATTR_HIDDEN) {
						strcat(attrStr, "h");
					}

					if (attr & ATTR_READ_ONLY) {
						strcat(attrStr, "R");
					}

					if (attr & ATTR_SYSTEM) {
						strcat(attrStr, "S");
					}

					if (attr & ATTR_VOLUME_ID) {
						strcat(attrStr, "V");
					}

					char crtTime[32];
					uint16_t cDate = file.head->CrtDate;
					uint16_t cTime = file.head->CrtTime;
					uint8_t cTimeTenths = file.head->CrtTimeTenth;
					uint32_t tenthSecondOfDay = (cTime >> 11) * 360000 
						+ (cTime >> 5 & 0x3F) * 6000 + (cTime & 0x1F) * 200 + cTimeTenths;
					sprintf(crtTime, "%04d-%02d-%02d %02d:%02d:%02d.%02d",
						(cDate >> 9) + 1980, cDate >> 5 & 0xF, cDate & 0x1F,
						tenthSecondOfDay / 360000, 
						tenthSecondOfDay % 360000 / 6000, 
						tenthSecondOfDay % 6000 / 100, tenthSecondOfDay % 100);
					uint32_t cluster = file.head->FstClusHI << 16 | file.head->FstClusLO;
					printf("Name: %s\nShort name: %s\nSize: %.4lfMB (%dB)\nCreated: %s\nAttribute: %s\nLocation: Cluster#%X @ %llX\n",
						file.name, parseShortName(file.head), file.head->FileSize / 1048576.0, file.head->FileSize,
						crtTime, attrStr, cluster, getClusterOffset(vol->PBR, cluster));
					break;
				}

				if (i == currentDir->fileCount - 1) {
					puts("Not found!");
				}
			}
		} else if (strcmp(cmd, "usefat") == 0) {
			uint32_t fatNum;
			scanf("%lu", &fatNum);
			if (fatNum >= 0 && fatNum < vol->PBR->NumFATs) {
				activeFATNum = fatNum;
			} else {
				puts("Such a FAT doesn't exist!");
			}
		} else if (strcmp(cmd, "help") == 0) {
			printCommands();
		} else if (strcmp(cmd, "exit") == 0) {
			break;
		} else {
			puts("Unknown command!");
		}
	}

	fclose(fp);
}

int main() {
	// Use UTF-8 charset
	system("chcp 65001");
	puts("C: Image reader console\nE: Partition images exporter\nQ: Quit");
	while (true) {
		switch (getchar() | 0x20) {
		case 'c':
			puts("Enter the name of the image: ");
			char imageName[1024] = { 0 };
			// Lazy~
			scanf("%s", imageName);
			openPartitionImage(imageName);
			break;
		case 'e':
			puts("Enter the name of the image: ");
			char in[1024] = { 0 };
			// Lazy~
			scanf("%s", in);
			FILE* fpHdd = fopen(in, "rb");
			PartitionTable* table = (PartitionTable*)malloc(sizeof(PartitionTable));
			readPartitionTableToBuf(fpHdd, table, true);
			for (int i = 0; i < table->partitionCount; i++) {
				Partition part = table->partitions[i];
				uint64_t bytes = (uint64_t)part.sectorCount * SECTOR_SIZE;
				printf("Vol # %-5d LBA: %#-12lX %15d Sectors %8.2lfGB (%15lldB)\n",
					i, part.startLBA, part.sectorCount, bytes / (double)(1 << 30), bytes);
			}

			puts("Enter the number of the partition to extract: ");
			int id;
			// Lazy~
			scanf("%u", &id);
			puts("Enter the destination: ");
			char dst[1024] = { 0 };
			// Lazy~
			scanf("%s", dst);
			extractPartition(fpHdd, &table->partitions[id], dst);
			fclose(fpHdd);
			break;
		case 'q':
			return 0;
		default:
			puts("C: Image reader console\nE: Partition images exporter\nQ: Quit");
			break;
		}
	}

	return 0;
}