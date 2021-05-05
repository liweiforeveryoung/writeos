//
// Created by liwei1 on 2021/5/5.
//

#include "file.h"

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