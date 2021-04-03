//
// Created by liwei1 on 2021/3/30.
//

#ifndef WRITEOS_BOOTPACK_H
#define WRITEOS_BOOTPACK_H

#include <stdio.h>
#include "global.h"

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
// 对于段的描述
struct SEGMENT_DESCRIPTOR {
    short limit_low;
    short base_low;     // 段的地址
    char base_mid;      // 段的地址
    char access_right;
    char limit_high;
    char base_high;     // 段的地址
};
// 为什么要分为3段呢？主要是为了与80286时代的程序兼容。有了这样的规格，80286用的操作系统，也可以不用修改就在386以后的CPU上运行了。

// 对于中断向量的描述
struct INTERRUPT_DESCRIPTOR {
    short offset_low;
    short selector;
    char dw_count;
    char access_right;
    short offset_high;
};

// 跑起来 yo yo yo checkout
void run();

// 初始化 global descriptor table（段表）
void init_gdt();

// 初始化中断向量表  interrupt descriptor table
void init_idt();

void init_mouse_cursor8(char *mouse, char bc);

// 打印鼠标
void print_mouse(unsigned char *vRam, short xSize, short x0, short y0,
                 short mouseWidth, short mouseHeight, const char *mouse);

void print_str(unsigned char *vRam, short xSize, short x0, short y0, const char *string, char color);

// 打印字体
void print_raw_char(unsigned char *vRam, short xSize, short x0, short y0, const char *c, char color);

void print_char(unsigned char *vRam, short xSize, short x0, short y0, char font, char color);

void box_fill8(unsigned char *vRam, int xSize, int x0, int y0, int x1, int y1, unsigned char c);

// 设置调色板
void init_palette(void);

// 将背景设置为白色
void set_white_background(void);

char *getFont(char c);

extern const char COL8_000000;            // 黑
extern const char COL8_FF0000;            // 亮红
extern const char COL8_00FF00;            // 亮绿
extern const char COL8_FFFF00;            // 亮黄
extern const char COL8_0000FF;            // 亮蓝
extern const char COL8_FF00FF;            // 亮紫
extern const char COL8_00FFFF;            // 浅亮蓝
extern const char COL8_FFFFFF;            // 白色
extern const char COL8_C6C6C6;            // 亮灰
extern const char COL8_840000;            // 暗红
extern const char COL8_008400;            // 暗绿
extern const char COL8_848400;            // 暗黄
extern const char COL8_000084;            // 暗青
extern const char COL8_840084;            // 暗紫
extern const char COL8_008484;            // 浅暗蓝
extern const char COL8_848484;            // 暗灰
extern const char COLOR_BLACK;
extern const char COLOR_WHITE;

/* int.c */
void init_pic(void);

extern const int PIC0_ICW1;
extern const int PIC0_OCW2;
extern const int PIC0_IMR;
extern const int PIC0_ICW2;
extern const int PIC0_ICW3;
extern const int PIC0_ICW4;
extern const int PIC1_ICW1;
extern const int PIC1_OCW2;
extern const int PIC1_IMR;
extern const int PIC1_ICW2;
extern const int PIC1_ICW3;
extern const int PIC1_ICW4;

#endif //WRITEOS_BOOTPACK_H
