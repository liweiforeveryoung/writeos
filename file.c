//
// Created by liwei1 on 2021/5/5.
//

#include "file.h"
#include "string.h"

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
    if (!strNEqual(file->name, name, dotIndex - 1)) {
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

char *GetFileAddress(struct FileInfo *file) {
    return (char *) (file->clustno * 512 + 0x003e00 + ADR_DISKIMG);
}

// 是否是最后一个文件
bool IsLastFile(struct FileInfo *file) {
    if (file->name[0] == Empty || file->name[0] == Deleted) {
        return true;
    }
    return false;
}