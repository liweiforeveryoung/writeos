//
// Created by liwei1 on 2021/4/11.
//

#include "sheet.h"
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
    set_sheet_pos(sheet, sheet->x0, sheet->y0);
}

// 设置图层的坐标
void set_sheet_pos(struct Sheet *sheet, short new_x, short new_y) {
    if (sheet->fixed) {
        // 固定的 sheet 无法被移动
        return;
    }
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
            for (level = control->topSheetIndex - 1; level >= 0; --level) {
                struct Sheet *sheet = control->sheets[level];
                if (get_color_from_sheet(sheet, x, y) != COLOR_TRANSPARENT) {
                    color = get_color_from_sheet(sheet, x, y);
                    break;
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
        sheet->fixed = false;
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

// 是否图层申请的内存空间
void free_sheet(struct Sheet *sheet) {
    memory_free(global_memory_manager, (unsigned int) sheet->buffer, sheet->width * sheet->height);
}

// 寻找 sheet 位置，如果找不到的话就返回 -1
int get_sheet_level(struct Sheet *sheet) {
    struct SheetControl *control = sheet->m_pControl;
    int level = 0;
    for (; level < control->topSheetIndex; ++level) {
        if (control->sheets[level] == sheet) {
            return level;
        }
    }
    return -1;
}

// 删除一个图层
bool delete_sheet(struct Sheet *sheet) {
    // 先找到该 sheet
    int sheetLevel = get_sheet_level(sheet);
    if (sheetLevel == -1) {
        return false;
    } else {
        struct SheetControl *control = sheet->m_pControl;
        // 之后将该 sheet 上面的 sheets 向下移动
        for (; sheetLevel < control->topSheetIndex - 1; ++sheetLevel) {
            control[sheetLevel] = control[sheetLevel + 1];
        }
        --control->topSheetIndex;
        free_sheet(sheet);
        return true;
    }
}

bool move_sheet_to_level(struct Sheet *sheet, int dstLevel);

// 将图层往上移动一层
bool move_up_sheet(struct Sheet *sheet) {
    return move_sheet_to_level(sheet, get_sheet_level(sheet) + 1);
}

// 将图层往下移动一层
bool move_down_sheet(struct Sheet *sheet) {
    return move_sheet_to_level(sheet, get_sheet_level(sheet) - 1);
}

// 将图层移动到指定的 level
bool move_sheet_to_level(struct Sheet *sheet, int dstLevel) {
    if (sheet->fixed) {
        // 固定的 sheet 无法被移动
        return false;
    }
    if (dstLevel >= sheet->m_pControl->topSheetIndex) {
        dstLevel = sheet->m_pControl->topSheetIndex - 1;
    }
    if (dstLevel < 0) {
        dstLevel = 0;
    }
    int originalLevel = get_sheet_level(sheet);
    struct SheetControl *control = sheet->m_pControl;
    for (; originalLevel < dstLevel; ++originalLevel) {
        control->sheets[originalLevel] = control->sheets[originalLevel + 1];
    }
    for (; originalLevel > dstLevel; --originalLevel) {
        control->sheets[originalLevel] = control->sheets[originalLevel - 1];
    }
    control->sheets[dstLevel] = sheet;
    return true;
}

char get_pixel_color(struct Sheet *sheet, int x, int y) {
    return sheet->buffer[y * sheet->width + x];
}

void set_pixel_color(struct Sheet *sheet, int x, int y, char color) {
    sheet->buffer[y * sheet->width + x] = color;
}