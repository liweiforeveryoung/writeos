//
// Created by liwei1 on 2021/3/30.
//

#ifndef WRITEOS_BOOTPACK_H
#define WRITEOS_BOOTPACK_H

#include <stdio.h>
#include "global.h"
#include "sheet.h"

struct BootInfo {
    char cyls;          // 磁盘的柱面数， ipl10.nas 中将柱面数写入了该地址
    char ledS;          // 指示灯的状态
    char vMode;         // 关于颜色数目的信息，颜色的位数
    char reverse;       // 保留的字段
    short screenX;      // 分辨率的 x (screen x)
    short screenY;      // 分辨率的 y (screen y)
    unsigned char *vRamAddr;     // 图像缓冲区的开始地址
};
extern struct BootInfo *const Boot_Info_Ptr;

// 跑起来 yo yo yo checkout
void run(struct SheetControl *control);

#endif //WRITEOS_BOOTPACK_H
