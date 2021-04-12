//
// Created by liwei1 on 2021/4/11.
//

#include "sheet.h"
#include "global.h"
#include "memorymanager.h"


#define transparent -1  // 透明的


int get_color_from_sheet(struct Sheet *sheet, short x, short y) {
    if ((x >= sheet->x0) && (x < sheet->x0 + sheet->width) &&
        (y >= sheet->y0) && (y < sheet->y0 + sheet->height)) {
        return sheet->buffer[(y - sheet->y0) * sheet->width + x - sheet->x0];
    }
    return transparent;
}

unsigned char *get_pix_addr(unsigned char *vRam, short screen_width, short x, short y) {
    return vRam + y * screen_width + x;
}


// 创建一个 sheet control
struct SheetControl *new_sheet_control(unsigned char *vRam, short screen_width, short screen_height) {
    struct SheetControl *control = (struct SheetControl *) memory_alloc(global_memory_manager,
                                                                        sizeof(struct SheetControl));
    control->screen_width = screen_width;
    control->screen_height = screen_height;
    control->vRam = vRam;
    control->topSheetIndex = 0;
    return control;
}

void free_sheet_control(struct SheetControl *control) {
    // todo
}

void sheet_control_draw(struct SheetControl *control) {
    short x = 0;
    short y = 0;
    int level = 0;
    int color;
    for (x = 0; x < control->screen_width; ++x) {
        for (y = 0; y < control->screen_height; ++y) {
            color = transparent;
            for (level = 0; level < control->topSheetIndex; level++) {
                struct Sheet *sheet = control->sheets[level];
                if (get_color_from_sheet(sheet, x, y) != transparent) {
                    color = get_color_from_sheet(sheet, x, y);
                }
            }
            if (color != transparent) {
                *get_pix_addr(control->vRam, control->screen_width, x, y) = color;
            }
        }
    }
}


// 在最上层创建一个图层
struct Sheet *create_sheet(struct SheetControl *control) {
    // todo
    if (control->topSheetIndex == MaxSheets) {
        return nullptr;
    } else {
        struct Sheet *sheet = (struct Sheet *) memory_alloc(global_memory_manager, sizeof(struct Sheet));
        control->sheets[control->topSheetIndex] = sheet;
        control->topSheetIndex++;
        return sheet;
    }
}

// 初始化 sheet
void init_sheet(struct Sheet *sheet, short x0, short y0, short width, short height) {
    sheet->x0 = x0;
    sheet->y0 = y0;
    sheet->width = width;
    sheet->height = height;
    sheet->buffer = (unsigned char *) memory_alloc(global_memory_manager, width * height);
}

// 将 sheet 的每个像素都设置为相同的颜色
void set_sheet_color(struct Sheet *sheet, unsigned char color) {
    short x, y;
    for (x = 0; x < sheet->width; x++) {
        for (y = 0; y < sheet->height; ++y) {
            sheet->buffer[y * sheet->width + x] = color;
        }
    }
}

// 删除一个图层
bool delete_sheet(struct SheetControl *control, struct Sheet *sheet) {
    // todo
    return false;
}

// 将图层往上移动一层
bool move_up_sheet(struct Sheet *sheet) {
    // todo
    return false;
}

// 将图层往下移动一层
bool move_down_sheet(struct Sheet *sheet) {
    // todo
    return false;
}
