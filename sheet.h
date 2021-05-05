//
// Created by liwei1 on 2021/4/11.
//

#ifndef WRITEOS_SHEET_H
#define WRITEOS_SHEET_H

#include "global.h"

#define MaxSheets 256

struct SheetControl;
// 图层
struct Sheet {
    short x0;   // 左上角 x
    short y0;   // 左上角 y
    short width;// 宽度
    short height;// 高度
    char *buffer;
    bool fixed;     // 图层是否的固定的，如果是固定的将无法拖动，以及上下移动
    struct SheetControl *m_pControl;    // 指向 control 的指针
};

struct SheetControl {
    unsigned char *vRam;
    short screen_height;
    short screen_width;
    int topSheetIndex;  // 最高图层的下一个图层
    struct Sheet *sheets[MaxSheets];    // todo 后续考虑用链表来存储图层信息
};

// 创建一个 sheet control
struct SheetControl *new_sheet_control(unsigned char *vRam, short screen_height, short screen_width);

// 绘制
void sheet_control_draw(struct SheetControl *control);

// 在最上层创建一个图层
struct Sheet *create_sheet(struct SheetControl *control, short x0, short y0, short width, short height);

// 将 sheet 的每个像素都设置为相同的颜色
void set_sheet_color(struct Sheet *sheet, char color);

// 刷新图层
void refresh_sheet(struct Sheet *sheet);

// 获取某个位置的 color
char get_pixel_color(struct Sheet *sheet, int x, int y);

// 设置某个位置的 color
void set_pixel_color(struct Sheet *sheet, int x, int y, char color);

// 设置图层的坐标
void set_sheet_pos(struct Sheet *sheet, short new_x, short new_y);

// 将图层往上移动一层
bool move_up_sheet(struct Sheet *sheet);
// 将图层往下移动一层
bool move_down_sheet(struct Sheet *sheet);

// 在指定位置处绘制一个 8 * 16 的小方块
void draw_8_16_block(struct Sheet *sheet, short x0, short y0, unsigned char color);

#endif //WRITEOS_SHEET_H
