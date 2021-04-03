//
// Created by liwei1 on 2021/3/30.
//

#include <stdio.h>
#include "bootpack.h"

const char COL8_000000 = 0;             // 黑
const char COL8_FF0000 = 1;             // 亮红
const char COL8_00FF00 = 2;             // 亮绿
const char COL8_FFFF00 = 3;             // 亮黄
const char COL8_0000FF = 4;             // 亮蓝
const char COL8_FF00FF = 5;             // 亮紫
const char COL8_00FFFF = 6;             // 浅亮蓝
const char COL8_FFFFFF = 7;             // 白色
const char COL8_C6C6C6 = 8;             // 亮灰
const char COL8_840000 = 9;             // 暗红
const char COL8_008400 = 10;            // 暗绿
const char COL8_848400 = 11;            // 暗黄
const char COL8_000084 = 12;            // 暗青
const char COL8_840084 = 13;            // 暗紫
const char COL8_008484 = 14;            // 浅暗蓝
const char COL8_848484 = 15;            // 暗灰
const char COLOR_BLACK = 0;
const char COLOR_WHITE = 7;

unsigned char *const VRam_Addr_Begin = (unsigned char *const) 0xa0000;
unsigned char *const VRam_Addr_End = (unsigned char *const) 0xaffff;
extern char hankaku[4096];

// 将背景设置为白色
void set_white_background(void) {
    unsigned char *p = VRam_Addr_Begin;
    for (; p < VRam_Addr_End; ++p) {
        *p = COLOR_WHITE;
    }
}

// 设置调色板
void init_palette(void) {
    const unsigned char palette_table[16][3] = {
            {0x00, 0x00, 0x00},   // 黑
            {0xff, 0x00, 0x00},   // 亮红
            {0x00, 0xff, 0x00},   // 亮绿
            {0xff, 0xff, 0x00},   // 亮黄
            {0x00, 0x00, 0xff},   // 亮蓝
            {0xff, 0x00, 0xff},   // 亮紫
            {0x00, 0xff, 0xff},   // 浅亮蓝
            {0xff, 0xff, 0xff},   // 白色
            {0xc6, 0xc6, 0xc6},   // 亮灰
            {0x84, 0x00, 0x00},   // 暗红
            {0x00, 0x84, 0x00},   // 暗绿
            {0x84, 0x84, 0x00},   // 暗黄
            {0x00, 0x00, 0x84},   // 暗青
            {0x84, 0x00, 0x84},   // 暗紫
            {0x00, 0x84, 0x84},   // 浅暗蓝
            {0x84, 0x84, 0x84}    // 暗灰
    };
    int i = 0;
    int eFlags = io_load_eflags();
    io_cli();
    io_out8(0x03c8, 0); // 从 0 号调色板开始设置
    for (; i < 16; ++i) {
        io_out8(0x03c9, palette_table[i][0]);   // 除以 4 的原因：http://oswiki.osask.jp/?VGA，因为高两位是无效的
        io_out8(0x03c9, palette_table[i][1]);
        io_out8(0x03c9, palette_table[i][2]);
    }
    io_store_eflags(eFlags);
}

void box_fill8(unsigned char *vRam, int xSize, unsigned char c, int x0, int y0, int x1, int y1) {
    int x, y;
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++)
            vRam[y * xSize + x] = c;
    }
}

void print_char(unsigned char *vRam, short xSize, short x0, short y0, const char font, char color) {
    print_raw_char(vRam, xSize, x0, y0, getFont(font), color);
}

// 打印字体
void print_raw_char(unsigned char *vRam, short xSize, short x0, short y0, const char *c, char color) {
    const short font_width = 8;
    const short font_height = 16;
    short x, y;
    for (y = y0; y < y0 + font_height; ++y) {
        unsigned char flag = 0x80;
        for (x = x0; x < x0 + font_width; ++x) {
            unsigned char *fontAddr = vRam + y * xSize + x;
            if ((c[y - y0] & flag) != 0) {
                *fontAddr = color;
            }
            flag = flag >> 1;
        }
    }
}

void print_str(unsigned char *vRam, short xSize, short x0, short y0, const char *string, char color) {
    const char *pChar = string;
    while (*pChar != '\0') {
        print_char(vRam, xSize, x0, y0, *pChar, color);
        x0 += 8;
        ++pChar;
    }
}

// 打印鼠标
void print_mouse(unsigned char *vRam, short xSize, short x0, short y0,
                 short mouseWidth, short mouseHeight, const char *mouse) {
    short extraY, extraX;
    for (extraY = 0; extraY < mouseHeight; ++extraY) {
        for (extraX = 0; extraX < mouseWidth; ++extraX) {
            short actualX = x0 + extraX;
            short actualY = y0 + extraY;
            unsigned char *actualAddr = vRam + actualY * xSize + actualX;
            *actualAddr = mouse[extraY * mouseWidth + extraX];
        }
    }
}

void init_mouse_cursor8(char *mouse, char bc) {
    static char cursor[16][16] = {
            "**************..",
            "*OOOOOOOOOOO*...",
            "*OOOOOOOOOO*....",
            "*OOOOOOOOO*.....",
            "*OOOOOOOO*......",
            "*OOOOOOO*.......",
            "*OOOOOOO*.......",
            "*OOOOOOOO*......",
            "*OOOO**OOO*.....",
            "*OOO*..*OOO*....",
            "*OO*....*OOO*...",
            "*O*......*OOO*..",
            "**........*OOO*.",
            "*..........*OOO*",
            "............*OO*",
            ".............***"
    };
    int x, y;

    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            if (cursor[y][x] == '*') {
                mouse[y * 16 + x] = COLOR_BLACK;
            }
            if (cursor[y][x] == 'O') {
                mouse[y * 16 + x] = COLOR_WHITE;
            }
            if (cursor[y][x] == '.') {
                mouse[y * 16 + x] = bc;
            }
        }
    }
}

char *getFont(char c) {
    return hankaku + c * 16;
}