//
// Created by liwei1 on 2021/4/3.
//

#ifndef WRITEOS_GRAPHIC_H
#define WRITEOS_GRAPHIC_H

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

#endif //WRITEOS_GRAPHIC_H
