//
// Created by liwei1 on 2021/4/7.
//

#include "memorymanager.h"

#define MEMORY_MANAGER_ADDR 0x003c0000
struct MemoryManager *global_memory_manager = (struct MemoryManager *) MEMORY_MANAGER_ADDR;

void manager_init(struct MemoryManager *manager) {
    manager->frees = 0;
    manager->lost_size = 0;
    manager->lost_num = 0;
}

// 报告空余内存大小的合计
unsigned int memory_total(struct MemoryManager *manager) {
    unsigned int total = 0;
    int i = 0;
    for (; i < manager->frees; ++i) {
        total += manager->free[i].size;
    }
    return total;
}

// 分配内存
unsigned int memory_alloc(struct MemoryManager *manager, unsigned int size) {
    int i = 0;
    unsigned int addr = 0;
    for (; i < manager->frees; ++i) {
        if (manager->free[i].size >= size) {
            manager->free[i].size -= size;
            addr = manager->free[i].addr;
            manager->free[i].addr += size;
            if (manager->free[i].size == 0) {
                // 刚好满足要求
                int j = i + 1;
                for (; j < manager->frees; ++j) {
                    manager->free[j - 1] = manager->free[j];
                }
                --manager->frees;
            }
            break;
        }
    }
    return addr;
}

// 归还内存
bool memory_free(struct MemoryManager *manager, unsigned int addr, unsigned int size) {
    int i = 0;
    // 先找到自己的位置
    // free[i - 1].addr < addr < free[i].addr
    for (; i < manager->frees; ++i) {
        if (manager->free[i].addr > addr) {
            break;
        }
    }
    struct FreeInfo *prev, *next;
    if (i - 1 >= 0) {
        prev = manager->free + i - 1;
    } else {
        prev = nullptr;
    }
    if (i + 1 < manager->frees) {
        next = manager->free + i + 1;
    } else {
        next = nullptr;
    }
    // 尝试去前面的合并
    if ((prev != nullptr) && (prev->addr + prev->size == addr)) {
        prev->size += size;
        // 尝试可否与后面的合并
        if ((next != nullptr) && (prev->addr + prev->size == next->addr)) {
            // oh yeah 可以合并
            prev->size += next->size;
            // 后面的全往前移动喽
            int j = i + 1;
            for (; j < manager->frees; ++j) {
                manager[i] = manager[j];
            }
            --manager->frees;
        }
        return true;
    }
    // 尝试与后面的合并
    if ((next != nullptr) && (addr + size == next->addr)) {
        next->addr -= size;
        next->size += size;
        return true;
    }
    // 尝试在 manager 中开辟一个单元来存储
    if (manager->frees < MEMORY_MANAGER_FREES) {
        // 后面的全往后移动一格
        int j = manager->frees;
        for (; j > i; --j) {
            manager->free[j] = manager->free[j - 1];
        }
        manager->free[i].addr = addr;
        manager->free[i].size = size;
        ++manager->frees;
        return true;
    } else {
        // 开辟失败就放弃咯，世上无难事，只要肯放弃
        ++manager->lost_num;
        manager->lost_size += size;
        return false;
    }
}