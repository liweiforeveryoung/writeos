//
// Created by liwei1 on 2021/4/7.
//

#ifndef WRITEOS_MEMORYMANAGER_H
#define WRITEOS_MEMORYMANAGER_H

#include "global.h"

#define MEMORY_MANAGER_FREES        4090
struct FreeInfo {
    unsigned int addr;
    unsigned int size;
};

struct MemoryManager {
    int frees;              // 用户的内存片段数量
    int lost_num;           // 内存归还失败的次数
    unsigned int lost_size; // 归还失败的内存之和
    struct FreeInfo free[MEMORY_MANAGER_FREES];
};

void manager_init(struct MemoryManager *manager);

// 分配内存
unsigned int memory_alloc(struct MemoryManager *manager, unsigned int size);

// 报告空余内存大小的合计
unsigned int memory_total(struct MemoryManager *manager);

#endif //WRITEOS_MEMORYMANAGER_H
