//
// Created by liwei1 on 2021/4/11.
//

#ifndef WRITEOS_SHEET_H
#define WRITEOS_SHEET_H
#define MaxSheets 256
// 图层
struct Sheet {
    short x0;
    short y0;
    short width;
    short height;
    int color;
};
struct SheetControl {
    unsigned char *vRam;
    short screen_height;
    short screen_width;
    int topSheetIndex;
    struct Sheet *sheets[MaxSheets];
};

// 创建一个 sheet control
struct SheetControl *new_sheet_control(unsigned char *vRam, short screen_height, short screen_width);

// 绘制
void sheet_control_draw(struct SheetControl *control);

// 在最上层创建一个图层
struct Sheet *create_sheet(struct SheetControl *control);

// 初始化 sheet
void init_sheet(struct Sheet *sheet, short x0, short y0, short width, short height, int color);

#endif //WRITEOS_SHEET_H
