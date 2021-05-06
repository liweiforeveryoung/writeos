//
// Created by liwei1 on 2021/5/5.
//

#include "file.h"
#include "string.h"
#include "memorymanager.h"

bool FormatFileInfoToBuffer(struct FileInfo *file, char *buffer) {
    if (file->name[0] == Empty || file->name[0] == Deleted) {
        return false;
    }
    sprintf(buffer, "filename.ext   %7d", file->size);
    if ((file->type & (Dir | NotFile)) == 0) {
        int y;
        for (y = 0; y < 8; y++) {
            buffer[y] = file->name[y];
        }
        buffer[9] = file->ext[0];
        buffer[10] = file->ext[1];
        buffer[11] = file->ext[2];
    } else {
        buffer[0] = '\0';
    }
    return true;
}

struct FileInfo *BaseFileInfoAddress = (struct FileInfo *) (ADR_DISKIMG + 0x002600);

static bool NameIsMatch(struct FileInfo *file, char *name) {
    int dotIndex = findIndex(name, '.');
    if (dotIndex <= 0) {
        return false;
    }
    if (!strNEqual(file->name, name, dotIndex)) {
        return false;
    }
    return strNEqual(file->ext, name + dotIndex + 1, 3);
}

struct FileInfo *FindFileByName(char *fileName) {
    struct FileInfo *file = BaseFileInfoAddress;
    while (!IsLastFile(file)) {
        if (NameIsMatch(file, fileName)) {
            return file;
        }
        file++;
    }
    return nullptr;
}

char *calFileAddressByClustNo(unsigned short clustNo) {
    return (char *) (clustNo * 512 + 0x003e00 + ADR_DISKIMG);
}

char *GetFileAddress(struct FileInfo *file) {
    return calFileAddressByClustNo(file->clustno);
}

// 是否是最后一个文件
bool IsLastFile(struct FileInfo *file) {
    if (file->name[0] == Empty || file->name[0] == Deleted) {
        return true;
    }
    return false;
}

unsigned char *FatTable;

// 初始化 fat table
void initFatTable() {
    // 因为只有 2880个扇区
    FatTable = (unsigned char *) memory_alloc(global_memory_manager, 2880);
    unsigned char *rawBitAddress = (unsigned char *) (ADR_DISKIMG + 0x000200);
    int i, j;
    for (i = 0, j = 0; i < 2880; i += 2, j += 3) {
        FatTable[i] = ((rawBitAddress[j + 1] << 8) & 0xf00) | rawBitAddress[j];
        FatTable[i + 1] = (rawBitAddress[j + 2] << 4) | (rawBitAddress[j + 1] >> 4);
    }
}

// 將 file 读取到 buffer 内
bool ReadFileIntoBuffer(struct FileInfo *file, char *buffer, int bufferSize) {
    int fileSize = file->size;
    if (bufferSize <= fileSize) {
        return false;
    }
    int i;
    for (i = 0; i < bufferSize; ++i) {
        buffer[i] = '\0';
    }
    char *begin;
    unsigned short curClusterNo = file->clustno;
    while (true) {
        begin = calFileAddressByClustNo(curClusterNo);
        int j;
        for (j = 0; j < fileSize; ++j) {
            buffer[j] = begin[j];
        }
        if (fileSize <= 512) {
            break;
        }
        fileSize -= 512;
        buffer += 512;
        curClusterNo = FatTable[curClusterNo];
    }
    return true;
}