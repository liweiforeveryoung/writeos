//
// Created by liwei1 on 2021/5/5.
//

#ifndef WRITEOS_FILE_H
#define WRITEOS_FILE_H

#include <stdio.h>  // for sprintf
#include "global.h"

struct FileInfo {
    unsigned char name[8], ext[3], type;
    char reserve[10];
    unsigned short time, date, clustno;
    unsigned int size;
};
#define ADR_DISKIMG        0x00100000

enum FileStatus {
    Empty = 0x00,   // 这一段不包含任何文件名信息
    Deleted = 0xe5, // 这个文件已经被删除了
};
enum FileType {
    ReadOnly = 0x01,    // 只读文件（不可写入）
    Hide = 0x02,    // 隐藏文件
    SysFile = 0x04, // 系统文件
    NotFile = 0x08, // 非文件信息（比如磁盘名称等）
    Dir = 0x10,     // 目录
    NormalFile = 0x20 // 普通文件
};

// 将文件的信息输入到 buffer 中，返回该文件是否存在
bool FormatFileInfoToBuffer(struct FileInfo *file, char *buffer);

// 根据文件名来查找文件
struct FileInfo *FindFileByName(char *fileName);

// 获取文件的真实地址
char *GetFileAddress(struct FileInfo *file);

extern struct FileInfo *BaseFileInfoAddress;

// 是否是最后一个文件
bool IsLastFile(struct FileInfo *file);

#endif //WRITEOS_FILE_H
