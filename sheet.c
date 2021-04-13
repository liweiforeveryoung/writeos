//
// Created by liwei1 on 2021/4/11.
//

#include "sheet.h"
#include "global.h"
#include "memorymanager.h"
#include "graphic.h"

void sheet_control_draw_sub_window(struct SheetControl *control, short x0, short y0, short x1, short y1);

char get_color_from_sheet(struct Sheet *sheet, short x, short y) {
    if ((x >= sheet->x0) && (x < sheet->x0 + sheet->width) &&
        (y >= sheet->y0) && (y < sheet->y0 + sheet->height)) {
        return sheet->buffer[(y - sheet->y0) * sheet->width + x - sheet->x0];
    }
    return COLOR_TRANSPARENT;
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

// 刷新图层
void refresh_sheet(struct Sheet *sheet) {
    sheet_control_draw(sheet->m_pControl);
}

// 设置图层的坐标
void set_sheet_pos(struct Sheet *sheet, short new_x, short new_y) {
    short x0 = min_short(sheet->x0, new_x);
    short x1 = max_short(sheet->x0, new_x) + sheet->width;
    short y0 = min_short(sheet->y0, new_y);
    short y1 = max_short(sheet->y0, new_y) + sheet->height;
    sheet->x0 = new_x;
    sheet->y0 = new_y;
    sheet_control_draw_sub_window(sheet->m_pControl, x0, y0, x1, y1);
}

void sheet_control_draw(struct SheetControl *control) {
    sheet_control_draw_sub_window(control, 0, 0, control->screen_width, control->screen_height);
}

// 重新绘制某一块区域
void sheet_control_draw_sub_window(struct SheetControl *control, short x0, short y0, short x1, short y1) {
    short x = 0;
    short y = 0;
    int level = 0;
    int color;
    for (x = x0; x < x1; ++x) {
        for (y = y0; y < y1; ++y) {
            color = COLOR_TRANSPARENT;
            for (level = 0; level < control->topSheetIndex; level++) {
                struct Sheet *sheet = control->sheets[level];
                if (get_color_from_sheet(sheet, x, y) != COLOR_TRANSPARENT) {
                    color = get_color_from_sheet(sheet, x, y);
                }
            }
            if (color != COLOR_TRANSPARENT) {
                *get_pix_addr(control->vRam, control->screen_width, x, y) = color;
            }
        }
    }
}


// 在最上层创建一个图层
struct Sheet *create_sheet(struct SheetControl *control, short x0, short y0, short width, short height) {
    if (control->topSheetIndex == MaxSheets) {
        return nullptr;
    } else {
        struct Sheet *sheet = (struct Sheet *) memory_alloc(global_memory_manager, sizeof(struct Sheet));
        sheet->x0 = x0;
        sheet->y0 = y0;
        sheet->width = width;
        sheet->height = height;
        sheet->buffer = (char *) memory_alloc(global_memory_manager, width * height);
        sheet->m_pControl = control;
        control->sheets[control->topSheetIndex] = sheet;
        control->topSheetIndex++;
        return sheet;
    }
}

// 将 sheet 的每个像素都设置为相同的颜色
void set_sheet_color(struct Sheet *sheet, char color) {
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