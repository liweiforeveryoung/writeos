//
// Created by liwei1 on 2021/5/5.
//

#ifndef WRITEOS_FILE_H
#define WRITEOS_FILE_H
struct FileInfo {
    unsigned char name[8], ext[3], type;
    char reserve[10];
    unsigned short time, date, clustno;
    unsigned int size;
};
#define ADR_DISKIMG        0x00100000

enum FileStatus {
    Empty = 0x00,
    Deleted = 0xe5,
};
enum FileType {
    ReadOnly = 0x01,
    Hide = 0x02,
    SysFile = 0x04,
    NotFile = 0x08,
    Dir = 0x10,
    NormalFile = 0x20
};

struct FileInfo *BaseFileInfoAddress = (struct FileInfo *) (ADR_DISKIMG + 0x002600);
#endif //WRITEOS_FILE_H