//
// Created by liwei1 on 2021/4/3.
//

#ifndef WRITEOS_GRAPHIC_H
#define WRITEOS_GRAPHIC_H

void init_mouse_cursor8(char *mouse, char bc);

// 打印鼠标
void print_mouse(unsigned char *vRam, short xSize, short x0, short y0,
                 short mouseWidth, short mouseHeight, const char *mouse);

void print_str(char *vRam, short xSize, short x0, short y0, const char *string, char color);

// 打印字体
void print_raw_char(char *vRam, short xSize, short x0, short y0, const char *c, char color);

void print_char(char *vRam, short xSize, short x0, short y0, const char font, char color);

void box_fill8(char *vRam, short xSize, int x0, int y0, int x1, int y1, unsigned char c);

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
extern const char COLOR_TRANSPARENT;    // 透明的

#endif //WRITEOS_GRAPHIC_H
