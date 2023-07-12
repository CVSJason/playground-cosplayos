#pragma once
#include "../utils.h"

struct Fat12FileInfo {
    char name[8], extension[3];
    byte type;
    char reserve[10];
    ushort time, date, clusterId;
    uint size;
};

struct ELF32Header {
    char magic[4];
    byte bits, endian, headVer, abi;
    char _unused0[8];
    ushort type;
    ushort instrSet;
    uint elfVer;
    uint entry;
    uint progHeaderOffset;
    uint sectionHeaderOffset;
    uint flags;
    ushort headerSize;
    ushort progHeaderSize;
    ushort progHeaderCount;
    ushort sectionHeaderSize;
    ushort sectionHeaderCount;
    ushort sectionNameStringTableOffset;
};

struct ELF32ProgramHeader {
    uint segmentType;
    uint offset;
    uint vaddress;
    uint _unused0;
    uint sizeInFile;
    uint sizeInMemory;
    uint flags;
    uint align;
};

struct ELF32SectionHeader {
    uint sectionName;
    uint sectionType;
    uint sectionFlags;
    uint sectionAddr;
    uint sectionOffset;
    uint sectionSize;
    uint sectionLink;
    uint sectionInfo;
    uint sectionAlign;
    uint sectionEntrySize;
};

void loadFat12To(int *fat, byte *source);
void loadFileFat12(int firstCluster, int size, byte *buffer, int *fat, byte *fileDataStart);
Fat12FileInfo *getFileInfo(char *fileName, Fat12FileInfo *startFileInfo);