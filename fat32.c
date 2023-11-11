#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "fat32.h"

const uint8_t ROOT_DIR_NAME[11] = {'/', ' ', ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' };
const unichar ROOT_DIR_NAME_UNICODE[2] = {'/', '\0'};

uint8_t activeFATNum = 0;

PBR* readPBR(FILE* fp) {
	PBR* result = (PBR*)malloc(sizeof(PBR));
	if (result == NULL) {
		return NULL;
	}

	// Well, we need to handle the fucking memory padding here...
	fseek(fp, 1, SEEK_CUR);
	fread(&result->jmpBootOffset, sizeof(result->jmpBootOffset), 1, fp);
	fseek(fp, 1, SEEK_CUR);
	fread(&result->OEMName, sizeof(result->OEMName), 1, fp);
	fread(&result->BytesPerSec, sizeof(result->BytesPerSec), 1, fp);
	fread(&result->SecPerClus, sizeof(result->SecPerClus), 1, fp);
	fread(&result->RsvdSecCnt, sizeof(result->RsvdSecCnt), 1, fp);
	fread(&result->NumFATs, sizeof(result->NumFATs), 1, fp);
	fread(&result->RootEntCnt, sizeof(result->RootEntCnt), 1, fp);
	fread(&result->TotSec16, sizeof(result->TotSec16), 1, fp);
	fread(&result->Media, sizeof(result->Media), 1, fp);
	fread(&result->FATSz16, sizeof(result->FATSz16), 1, fp);
	fread(&result->SecPerTrk, sizeof(result->SecPerTrk), 1, fp);
	fread(&result->NumHeads, sizeof(result->NumHeads), 1, fp);
	fread(&result->HiddSec, sizeof(result->HiddSec), 1, fp);
	fread(&result->TotSec32, sizeof(result->TotSec32), 1, fp);
	fread(&result->FATSz32, sizeof(result->FATSz32), 1, fp);
	fread(&result->ExtFlags, sizeof(result->ExtFlags), 1, fp);
	fread(&result->FSVer, sizeof(result->FSVer), 1, fp);
	fread(&result->RootClus, sizeof(result->RootClus), 1, fp);
	fread(&result->FSInfo, sizeof(result->FSInfo), 1, fp);
	fread(&result->BkBootSec, sizeof(result->BkBootSec), 1, fp);
	fread(&result->Reserved, sizeof(result->Reserved), 1, fp);
	fread(&result->DrvNum, sizeof(result->DrvNum), 1, fp);
	fread(&result->Reserved1, sizeof(result->Reserved1), 1, fp);
	fread(&result->BootSig, sizeof(result->BootSig), 1, fp);
	fread(&result->VolID, sizeof(result->VolID), 1, fp);
	fread(&result->VolLab, sizeof(result->VolLab), 1, fp);
	fread(&result->FilSysType, sizeof(result->FilSysType), 1, fp);
	fseek(fp, 422, SEEK_CUR);
	return result;
}

FSInfo* readFSInfo(FILE* fp) {
	FSInfo* result = (FSInfo*)malloc(sizeof(FSInfo));
	if (result == NULL) {
		return NULL;
	}
	
	// Luckily the structure here only contains uint32_t
	fseek(fp, 488, SEEK_CUR);
	fread(result, sizeof(FSInfo), 1, fp);
	fseek(fp, 16, SEEK_CUR);
	return result;
}

DirEntry* readDirEntryToBuf(FAT32FilePointer* ffp, DirEntry* result) {
	// But handle the bloody padding here with FAT32Fp again...
	freadFAT32(&result->Name, sizeof(result->Name), 1, ffp, false);
	freadFAT32(&result->Attr, sizeof(result->Attr), 1, ffp, false);
	freadFAT32(&result->NTRes, sizeof(result->NTRes), 1, ffp, false);
	freadFAT32(&result->CrtTimeTenth, sizeof(result->CrtTimeTenth), 1, ffp, false);
	freadFAT32(&result->CrtTime, sizeof(result->CrtTime), 1, ffp, false);
	freadFAT32(&result->CrtDate, sizeof(result->CrtDate), 1, ffp, false);
	freadFAT32(&result->LstAccDate, sizeof(result->LstAccDate), 1, ffp, false);
	freadFAT32(&result->FstClusHI, sizeof(result->FstClusHI), 1, ffp, false);
	freadFAT32(&result->WrtTime, sizeof(result->WrtTime), 1, ffp, false);
	freadFAT32(&result->WrtDate, sizeof(result->WrtDate), 1, ffp, false);
	freadFAT32(&result->FstClusLO, sizeof(result->FstClusLO), 1, ffp, false);
	freadFAT32(&result->FileSize, sizeof(result->FileSize), 1, ffp, false);
	return result;
}

DirEntry* readDirEntry(FAT32FilePointer* ffp) {
	DirEntry* result = (DirEntry*)malloc(sizeof(DirEntry));
	if (result == NULL) {
		return NULL;
	}

	return readDirEntryToBuf(ffp, result);
}

LNameDirEntry* readLNameDirEntryToBuf(FAT32FilePointer* ffp, LNameDirEntry* result) {
	// And again...
	freadFAT32(&result->Ord, sizeof(result->Ord), 1, ffp, false);
	freadFAT32(&result->Name1, sizeof(result->Name1), 1, ffp, false);
	freadFAT32(&result->Attr, sizeof(result->Attr), 1, ffp, false);
	freadFAT32(&result->Type, sizeof(result->Type), 1, ffp, false);
	freadFAT32(&result->Chksum, sizeof(result->Chksum), 1, ffp, false);
	freadFAT32(&result->Name2, sizeof(result->Name2), 1, ffp, false);
	freadFAT32(&result->FstClusLO, sizeof(result->FstClusLO), 1, ffp, false);
	freadFAT32(&result->Name3, sizeof(result->Name3), 1, ffp, false);
	return result;
}

LNameDirEntry* readLNameDirEntry(FAT32FilePointer* ffp) {
	LNameDirEntry* result = (LNameDirEntry*)malloc(sizeof(LNameDirEntry));
	if (result == NULL) {
		return NULL;
	}

	return readLNameDirEntryToBuf(ffp, result);
}

/* Parse the short name of a DirEntry*/
void parseShortNameToBuf(DirEntry* dirPtr, unsigned char* buf) {
	// Can u understand such a pile of code?
	int dotPos = 8;
	for (int i = 7; i >= 0; i--) {
		char c;
		if ((c = dirPtr->Name[i]) == ' ') {
			dotPos--;
			continue;
		}

		buf[i] = c;
	}

	int endPos = dotPos + 1;
	for (int i = 10; i >= 8; i--) {
		char c;
		if ((c = dirPtr->Name[i]) == 0x20) {
			continue;
		}

		buf[dotPos + i - 7] = c;
		endPos++;
	}

	if (endPos != dotPos + 1) {
		buf[dotPos] = '.';
	} else {
		endPos--;
	}

	buf[endPos] = '\0';
	return buf;
}

/* Parse the short name of a DirEntry*/
char* parseShortName(DirEntry* dirPtr) {
	unsigned char* buf = (unsigned char*)malloc(12);
	if (buf == NULL) {
		return NULL;
	}

	parseShortNameToBuf(dirPtr, buf);
	return buf;
}

inline uint32_t getClusterSize(PBR* PBR) {
	return PBR->BytesPerSec * PBR->SecPerClus;
}

inline uint64_t getClusterOffset(PBR* PBR, uint32_t cluster) {
	return getClusterSize(PBR) * ((uint64_t)cluster - 2) + (uint64_t)(PBR->RsvdSecCnt + PBR->FATSz32 * PBR->NumFATs) * PBR->BytesPerSec;
}

uint32_t estimateFileSize(FILE* fp, PBR* PBR, uint32_t startingCluster) {
	uint64_t originalFp;
	// We cannot use ftell & fseek dut to integer overflow
	fgetpos(fp, &originalFp);
	uint32_t clusterCount = 0;
	uint32_t fatEntry = startingCluster;
	do {
		uint64_t fatOffset = getFATLocation(fatEntry, PBR);
		fsetpos(fp, &fatOffset);
		fread(&fatEntry, 4, 1, fp);
		clusterCount++;
	} while (fatEntry != FAT_VALUE_FREE && fatEntry != FAT_VALUE_EOF && fatEntry != FAT_VALUE_BAD);
	fsetpos(fp, &originalFp);
	return clusterCount * getClusterSize(PBR);
}

FAT32FilePointer* newFAT32Fp(FILE* fp, LNameDirEntryGroup* file, PBR* context, bool unknownSize) {
	FAT32FilePointer* result = (FAT32FilePointer*)malloc(sizeof(FAT32FilePointer));
	if (result == NULL) {
		return NULL;
	}

	result->fileSizeUnknown = unknownSize;
	result->cluster = file->head->FstClusHI << 16 | file->head->FstClusLO;
	result->offsetInCluster = 0;
	result->fp = fp;
	result->context = context;
	result->isAtDetectedBadCluster = false;
	result->totalBytes = result->fileSizeUnknown ? 
			estimateFileSize(fp, context, result->cluster) : file->head->FileSize;
	result->clusterOrdinal = 0;
	result->prevCluster = NULL_CLUSTER;
	resyncCstdioFpOfFFP(result);
	return result;
}

inline uint32_t ftellFAT32(FAT32FilePointer* ffp) {
	return ffp->clusterOrdinal * getClusterSize(ffp->context) + ffp->offsetInCluster;
}

inline bool canReadWhenOffseted(FAT32FilePointer* ffp, int64_t offset) {
	int64_t offseted = offset + ftellFAT32(ffp);
	return offseted < ffp->totalBytes && offseted >= 0;
}

inline bool canReadAt(FAT32FilePointer* ffp, int64_t pos) {
	return pos < ffp->totalBytes && pos >= 0 || ffp->fileSizeUnknown;
}

inline void resyncCstdioFpOfFFP(FAT32FilePointer* ffp) {
	uint64_t offset = getClusterOffset(ffp->context, ffp->cluster) + ffp->offsetInCluster;
	fsetpos(ffp->fp, &offset);
}

/* 
  Steps to the next cluster.
  Returns true if the file has the next cluster, or false is EOF or a bad cluster is encountered.
  The pointer remains untouched if the function returns false.
*/
uint8_t stepToTheNextCluster(FAT32FilePointer* ffp) {
	FILE* fp = ffp->fp;
	uint64_t fatLocation = getFATLocation(ffp->cluster, ffp->context);
	fsetpos(fp, &fatLocation);
	uint32_t fatEntry;
	fread(&fatEntry, 4, 1, fp);
	if (fatEntry == FAT_VALUE_BAD) {
		ffp->isAtDetectedBadCluster = true;
		return STEP_CLUSTER_ERROR;
	} else if (fatEntry == FAT_VALUE_EOF) {
		return STEP_CLUSTER_EOF;
	}

	ffp->prevCluster = ffp->cluster;
	ffp->cluster = fatEntry;
	ffp->offsetInCluster = 0;
	resyncCstdioFpOfFFP(ffp);
	return STEP_CLUSTER_SUCCESS;
}

/*
  Moving the pointer backwards may be much more expensive than moving forward.
  We don't use int32_t as the range of its values may be insufficient.
  Returns false when ffp is going to be moved out of the file or any error has occurred.
*/
bool fseekFAT32(FAT32FilePointer* ffp, int64_t offset, int mode, bool ignoreErrors) {
	if (mode == SEEK_CUR ? !canReadWhenOffseted(ffp, offset) : !canReadAt(ffp, offset)) {
		return false;
	}

	if (mode == SEEK_SET) {
		// relativize the offset so that we can use the code for SEEK_CUR regardless the mode.
		offset -= ftellFAT32(ffp);
	}

	uint32_t mayNewOffset = offset + ffp->offsetInCluster;
	uint32_t clusterSize = getClusterSize(ffp->context);
	if (mayNewOffset < clusterSize && mayNewOffset >= 0) {
		ffp->offsetInCluster = mayNewOffset;
		// Here we synchronize the internal fp in an optimized way, so we need't call resyncCstdioFpOfFFP()
		fseek(ffp->fp, offset, SEEK_CUR);
		return true;
	} else if (offset > 0) {
		uint32_t remainingInBeginningCluster = (clusterSize - ffp->offsetInCluster);
		uint32_t fullClusters = (offset - remainingInBeginningCluster) / clusterSize;
		for (uint32_t i = 0; i < fullClusters + 1; i++) {	// We should skip over fullCluster + 1 cluster borders
			uint8_t stepErrno = stepToTheNextCluster(ffp);
			if (stepErrno == STEP_CLUSTER_ERROR && !ignoreErrors
					|| stepErrno == STEP_CLUSTER_EOF && !ffp->fileSizeUnknown) {
				ffp->isAtDetectedBadCluster = stepErrno == STEP_CLUSTER_ERROR;
				resyncCstdioFpOfFFP(ffp);
				return false;
			}
		}

		ffp->offsetInCluster = (offset - remainingInBeginningCluster - fullClusters) % clusterSize;
		resyncCstdioFpOfFFP(ffp);
		return true;
	} else {
		// offset < 0, if nothing is wrong
		if (offset + ftellFAT32(ffp) < clusterSize) {
			// Naive optimization
			ffp->offsetInCluster = offset + ftellFAT32(ffp);
			ffp->cluster = ffp->beginningCluster;
			ffp->prevCluster = NULL_CLUSTER;
			ffp->clusterOrdinal = 0;
			resyncCstdioFpOfFFP(ffp);
			return true;
		} else if (offset + ffp->offsetInCluster + clusterSize >= 0 && ffp->prevCluster != NULL_CLUSTER) {
			ffp->offsetInCluster = offset + ffp->offsetInCluster + clusterSize;
			ffp->cluster = ffp->prevCluster;
			ffp->prevCluster = NULL_CLUSTER;
			ffp->clusterOrdinal = 0;
			resyncCstdioFpOfFFP(ffp);
			return true;
		}

		// We simply do some inefficiency transformation... (lazy \_/)
		// Actually we seeks for the satisfying cluster from the beginning of the file.
		// Hopefully it has no chance to loop forever...
		ffp->offsetInCluster = 0;
		ffp->cluster = ffp->beginningCluster;
		ffp->prevCluster = NULL_CLUSTER;
		ffp->clusterOrdinal = 0;
		return fseekFAT32(ffp, ftellFAT32(ffp) + offset, SEEK_CUR, ignoreErrors);
	}
}

/* 
  But anyway, we would usually expect ffp->fp to be at the next byte of what we read after execution.
  The pointer stoppes at the end of the last readable cluster if any error is encountered, 
  but I have to say the behavior of the function is undefined after ignoring errors;
*/
uint32_t freadFAT32(void *buf, uint32_t elementSize, uint32_t elementNum, FAT32FilePointer *ffp, bool ignoreErrors) {
	// Using uint32_t is fine since files larger than 4GB are not allowed on FAT32 volumes.
	uint32_t size = elementSize * elementNum;
	if (size + ftellFAT32(ffp) > ffp->totalBytes && !ffp->fileSizeUnknown) {
		// Out of bound
		// printf("Out of bound: %d + %d > %d", size, ftellFAT32(ffp), ffp->totalBytes);
		size = ffp->totalBytes - ftellFAT32(ffp) / elementSize * elementSize;
	}

	if (size + ffp->offsetInCluster < getClusterSize(ffp->context)) {
		// Still in the same cluster
		ffp->offsetInCluster += size;
		return fread(buf, elementSize, elementNum, ffp->fp);
	} else {
		// We should step to the next cluster
		uint64_t offseted = buf;	// Sadly, arithmatic operations on void* doesn't compile on VS2022
		uint32_t clusterSize = getClusterSize(ffp->context);
		uint32_t remainingInBeginningCluster = (clusterSize - ffp->offsetInCluster);
		offseted += fread((void*)offseted, 1, remainingInBeginningCluster, ffp->fp);
		uint32_t remaining = size - remainingInBeginningCluster;
		uint32_t fullClusters = (size - remainingInBeginningCluster) / clusterSize;
		for (uint32_t i = 0; i < fullClusters + 1; i++) {
			bool stepErrno = stepToTheNextCluster(ffp);
			if (stepErrno == STEP_CLUSTER_ERROR && !ignoreErrors) {
				ffp->clusterOrdinal += i;
				ffp->offsetInCluster = clusterSize - 1;
				return offseted - (uint64_t)buf;
			} else if (stepErrno == STEP_CLUSTER_EOF && ffp->fileSizeUnknown) {	// TODO
				return offseted - (uint64_t)buf;
			} else {
				uint32_t step = remaining >= clusterSize ? clusterSize : remaining;
				fread((void*)offseted, 1, step, ffp->fp);
				offseted += step;
				remaining -= step;
			}
		}

		uint32_t bytesInTheLastCluster = 
			(size - remainingInBeginningCluster - fullClusters * getClusterSize(ffp->context)) % clusterSize;
		ffp->offsetInCluster = bytesInTheLastCluster;
		uint32_t pos0 = ftell(ffp->fp);
		resyncCstdioFpOfFFP(ffp);
		uint32_t pos1 = ftell(ffp->fp);
		if (pos0 != pos1) {
			puts("ERROR");
		}

		return offseted - (uint64_t)buf;
	}
}

unsigned char* encodeUtf8(unichar* in) {
	unichar c;
	uint32_t len = 0;
	int i = 0;
	while ((c = in[i++]) != 0x0000) {
		len++;
	}

	unsigned char* result = (unsigned char*)malloc(len * 3 + 1);
	if (result == NULL) {
		return NULL;
	}

	uint32_t inPos = 0;
	uint32_t outPos = 0;
	while ((c = in[inPos++]) != 0x0000) {
		if (c <= 0x7F) {
			result[outPos++] = c;
		} else if (c <= 0x7FF) {
			result[outPos++] = 0xC0 | c >> 6;
			result[outPos++] = 0x80 | c & 0x3F;
		} else {
			result[outPos++] = 0xE0 | c >> 12;
			result[outPos++] = 0x80 | c >> 6 & 0x3F;
			result[outPos++] = 0x80 | c & 0x3F;
		}
	}

	result[outPos] = '\0';
	return result;
}

/*
  Initially, the file pointer should point to the first byte of an entry.
  And when shortNameEntry is true, it is up to us to make sure that the entry is a short name entry.
*/
LNameDirEntryGroup* readLNameDirEntryGroup(FAT32FilePointer* ffp, bool shortNameEntry) {
	LNameDirEntryGroup* result = (LNameDirEntryGroup*)malloc(sizeof(LNameDirEntryGroup));
	unichar* name = (unichar*)malloc(514);	// Not 114514
	// A group may contain 19 entries at most.
	LNameDirEntry* entries = (LNameDirEntry*)malloc(19 * sizeof(LNameDirEntry));
	if (name == NULL || result == NULL || entries == NULL) {
		return NULL;
	}

	if (!shortNameEntry) {
		// If we can't ensure that the first entry is an short entry, then find one.
		// Maybe less efficiency, but luckily we don't use it here.
		// Idk if it works, hopefully it'll do a good job...
		while (true) {
			// Move to the attribute byte first.
			fseekFAT32(ffp, 0xB, SEEK_CUR, false);
			uint8_t attr;
			freadFAT32(&attr, sizeof(uint8_t), 1, ffp, false);
			if (attr == ATTR_LONG_NAME) {
				// Well, a LName entry, then we should check the next entry.
				fseekFAT32(ffp, 0x14, SEEK_CUR, false);
				continue;
			} else {
				// Finally a short name entry is found, let us move ffp to its head.
				// Maybe i know why such a reversed storage order is employed.
				fseekFAT32(ffp, -0xC, SEEK_CUR, false);
				break;
			}
		}
	}

	result->head = readDirEntry(ffp);
	uint32_t endPos = ftellFAT32(ffp);
	int ordinal = 0;
	int nameLength = 0;
	bool hasAnyLNameEntry = false;
	// First we should move back to the head of the short name entry
	fseekFAT32(ffp, -0x20, SEEK_CUR, false);
	// And then the previous entry (effectively the next)
	while (fseekFAT32(ffp, -0x20, SEEK_CUR, false)) {
		if (ordinal >= 19) {
			// Prevent the access violation caused by broken entries.
			hasAnyLNameEntry = false;
			break;
		}

		readLNameDirEntryToBuf(ffp, &entries[ordinal]);
		LNameDirEntry entry = entries[ordinal];
		if (entry.Attr != ATTR_LONG_NAME) {
			// We don't revert the reading process as the value we readd just now should be seen invalid.
			break;
		}

		// We can use memcpy() as we will search for the tail of the name later.
		if (hasAnyLNameEntry) {
			memcpy(&name[nameLength] + 13, &name[nameLength], 13 * sizeof(unichar));
		}

		memcpy(&name[nameLength], entry.Name1, sizeof(entry.Name1));
		nameLength += 5;
		memcpy(&name[nameLength], entry.Name2, sizeof(entry.Name2));
		nameLength += 6;
		memcpy(&name[nameLength], entry.Name3, sizeof(entry.Name3));
		nameLength += 2;
		ordinal++;
		// Move ffp onto the head of the current entry
		// Maybe we should do some magic... And yeah, actually we did...
		fseekFAT32(ffp, -0x20, SEEK_CUR, false);
		hasAnyLNameEntry = true;
		if (entry.Ord & 0x40) {
			break;
		}
	}

	if (hasAnyLNameEntry) {
		// Find the tail or the long name, if any
		result->entries = entries;
		result->entryCount = ordinal;
		bool exactlyFit = true;
		int trailingChars = 0;
		for (int trailingChars = 0; trailingChars <= 12; trailingChars++) {
			unichar c = name[nameLength - trailingChars - 1];
			if (c != 0xFFFF) {
				exactlyFit = false;
			} else {
				if (c != 0x0000 && c != ('.' << 8)) {
					break;
				}
			}
		}

		if (exactlyFit) {
			trailingChars = 0;
		}

		nameLength -= trailingChars;
		name[nameLength] = 0x0000;
	} else {
		// If the file has no long name, use its short name.
		// We cannot use parseShortNameToBuf directly as it parse the name into ASCII nstead of UNICODE
		// But after some post processing everything will be fine
		char* shortNameAscii = parseShortName(result->head);
		uint32_t length = strlen(shortNameAscii);
		for (uint32_t i = 0; i < length; i++) {
			name[i] = shortNameAscii[i];
		}

		name[length] = 0x0000;
	}

	if (FILE_NAME_UTF8) {
		if ((result->name = encodeUtf8(name)) == NULL) {
			return NULL;
		}
	} else {
		result->name = name;
	}
	
	fseekFAT32(ffp, endPos, SEEK_SET, false);
	return result;
}

Dir* readDir(FILE* fp, LNameDirEntryGroup* dirFile, PBR* PBR) {
	if (!dirFile->head->Attr & ATTR_DIRECTORY) {
		return NULL;
	}

	// XXX: Should we read the whole dir file all at once?
	// Maybe it will be fine, as the size of directories is limited to 2MB
	Dir* result = (Dir*)malloc(sizeof(Dir));
	FAT32FilePointer* ffp = newFAT32Fp(fp, dirFile, PBR, true);
	uint32_t size = ffp->totalBytes / 32;
	LNameDirEntryGroup* files = (LNameDirEntryGroup*)malloc(size * sizeof(LNameDirEntryGroup));
	if (result == NULL || files == NULL || ffp == NULL) {
		return NULL;
	}

	// The file pointer should point to the first byte of corresponding directory file.
	uint32_t count = 0;
	for (uint32_t i = 0; i < size; i++) {
		// Move to the attribute byte first.
		fseekFAT32(ffp, 0xB, SEEK_CUR, false);
		uint8_t attr;
		freadFAT32(&attr, sizeof(uint8_t), 1, ffp, false);
		if (attr == ATTR_LONG_NAME) {
			// Well, we should move onto the next entry now.
			// As we are at 0xC now, we should move 0x14 bytes forward.
			fseekFAT32(ffp, 0x14, SEEK_CUR, false);
			continue;
		}

		fseekFAT32(ffp, -0xC, SEEK_CUR, false);
		// Here the ffp is at the first byte of an entry, but we cannot ensure that the entry is a short name entry.
		// Thus, some special handling is necessary...
		LNameDirEntryGroup* singleFile = readLNameDirEntryGroup(ffp, true);
		if (singleFile == NULL) {
			return NULL;
		}

		if (singleFile->head->Name[0] == (KANJI_CHARSET_NAME_FLAG_HANDLING ? 0x05 : 0xE5)) {
			// The fxxking operator priority...
			// 0xE5 signals a file has been deleted, i guess.
			continue;
		} else if (singleFile->head->Name[0] == 0x00) {
			// A nice marker for the ending of the root directory?
			break;
		}

		files[count++] = *singleFile;
	}

	//void* shrunked = realloc(files, count * sizeof(uint32_t));
	//if (shrunked == NULL) {
	//	return NULL;
	//}

	result->fileCount = count;
	result->files = files;
	result->self = dirFile;
	return result;
}

Dir* readRootDir(FILE* fp, PBR *PBR) {
	uint64_t clusterOffset = getClusterOffset(PBR, PBR->RootClus);
	fsetpos(fp, &clusterOffset);
	LNameDirEntryGroup *rootEntry = (LNameDirEntryGroup*)malloc(sizeof(LNameDirEntry));
	DirEntry* fabricatedRootDirEntry = (DirEntry*)malloc(sizeof(DirEntry));
	if (rootEntry == NULL || fabricatedRootDirEntry == NULL) {
		return NULL;
	}

	rootEntry->entries = NULL;
	rootEntry->entryCount = 0;
	rootEntry->name = &ROOT_DIR_NAME_UNICODE;
	rootEntry->head = fabricatedRootDirEntry;
	// We cannot use PBR->RootEntCnt as it is always 0 on FAT32 valumes.
	memcpy(&rootEntry->head->Name, ROOT_DIR_NAME, sizeof(ROOT_DIR_NAME));
	rootEntry->head->FileSize = 0;
	rootEntry->head->FstClusHI = PBR->RootClus >> 16;
	rootEntry->head->FstClusLO = PBR->RootClus & 0xFFFF;
	rootEntry->head->Attr = ATTR_DIRECTORY;
	return readDir(fp, rootEntry, PBR);
}

inline fpos_t getFATLocation(uint32_t cluster, PBR *PBR) {
	return cluster * 4L + PBR->RsvdSecCnt * PBR->BytesPerSec;
}

inline fpos_t getFATBeginLocation(PBR* PBR) {
	return PBR->RsvdSecCnt * PBR->BytesPerSec;
}

uint64_t calculateFreeSpaceUncached(FILE *fp, PBR *PBR) {
	fpos_t initialFp = ftell(fp);
	uint64_t fatLocation = getFATBeginLocation(PBR);
	fsetpos(fp, &fatLocation);
	uint32_t totalClusters = PBR->TotSec32 / PBR->SecPerClus;
	uint32_t freeClusters = 0;
	for (uint32_t cluster = 0; cluster < totalClusters; cluster++) {
		uint32_t fatEntry;
		fread(&fatEntry, 4, 1, fp);
		if (fatEntry == FAT_VALUE_FREE) {
			freeClusters++;
		}
	}

	return freeClusters * getClusterSize(PBR);
}

FAT32Partition* readPartition(FILE* fp) {
	FAT32Partition* vol = (FAT32Partition*)malloc(sizeof(FAT32Partition));
	uint32_t capicity = 256;
	uint32_t* badClusters = (uint32_t*)malloc(capicity * sizeof(uint32_t));
	if (vol == NULL) {
		return NULL;
	}

	PBR* PBR = readPBR(fp);
	vol->PBR = PBR;
	vol->rootDir = readRootDir(fp, PBR);
	vol->freeSpace = calculateFreeSpaceUncached(fp, PBR);
	// Couldn't overflow
	fseek(fp, PBR->BytesPerSec * 7, SEEK_SET);
	vol->fsInfo = readFSInfo(fp);
	// It couldn't overflow, i guess
	fseek(fp, getFATBeginLocation(PBR), SEEK_SET);
	uint32_t totalClusters = PBR->TotSec32 / PBR->SecPerClus;
	uint32_t badClusterCount = 0;
	bool recordBadClusters = false;
	for (uint32_t cluster = 0; cluster < totalClusters; cluster++) {
		uint32_t fatEntry;
		fread(&fatEntry, 4, 1, fp);
		// Nasty embedding blocks...
		if (fatEntry == FAT_VALUE_BAD) {
			if (recordBadClusters) {
				if (badClusterCount >= capicity) {
					bool reallocSuccess = realloc(badClusters, capicity *= 2) != NULL;
					if (reallocSuccess) {
						recordBadClusters[(int32_t*)badClusterCount] = cluster;
					} else {
						recordBadClusters = false;
					}
				} else {
					recordBadClusters[(int32_t*)badClusterCount] = cluster;
				}
			}

			badClusterCount++;
		}
	}

	vol->badClusterCount = badClusterCount;
	vol->badClusters = badClusters;
	return vol;
}