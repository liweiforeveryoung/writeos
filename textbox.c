//
// Created by liwei1 on 2021/5/5.
//
#include "sheet.h"
#include "graphic.h"
#include "memorymanager.h"
#include "textbox.h"

// 处理 enter 键
void handle_enter(struct TextBox *textBox) {
    // enter 的時候必須 flush 一次
    handle_redraw(textBox);
    // 将当前行的最后一个白色小方块删掉
    draw_8_16_block(textBox->sheet, textBox->x0 + textBox->key_cursor_x * 8, textBox->current_line_y,
                    COLOR_BLACK);
    textBox->current_line_y += 16;
    if (textBox->current_line_y > textBox->y1 - 16) {
        // 意味着需要滚动了
        short x, y;
        char color;
        for (y = textBox->y0; y < textBox->y1 - 16; ++y) {
            for (x = textBox->x0; x < textBox->x1; ++x) {
                color = get_pixel_color(textBox->sheet, x, y + 16);
                set_pixel_color(textBox->sheet, x, y, color);
            }
        }
        textBox->current_line_y = textBox->y1 - 16;
    }
    textBox->key_cursor_x = 0;
}

// 处理 backspace 键盘
void handle_backspace(struct TextBox *textBox) {
    --textBox->key_cursor_x;
    if (textBox->key_cursor_x < 0) {
        textBox->key_cursor_x = 0;
    }
    handle_redraw(textBox);
}

// 处理新的字符输入
void handle_new_char_come(struct TextBox *textBox, char newChar) {
    textBox->line_buffer[textBox->key_cursor_x] = newChar;
    ++textBox->key_cursor_x;
    // 最多打印十个字符
    if (textBox->key_cursor_x > textBox->max_char_count_of_line) {
        handle_enter(textBox);
    }
}

// 更新一行字符串
void handle_new_line(struct TextBox *textBox, char *line) {
    int i;
    for (i = 0; line[i] != '\0'; i++) {
        handle_new_char_come(textBox, line[i]);
    }
    handle_enter(textBox);
}

// 打印一个字符，之后回车，之后刷新
void print_char_in_console_and_redraw(struct TextBox *textBox, char ch) {
    handle_new_char_come(textBox, ch);
    handle_enter(textBox);
    handle_redraw(textBox);
}

// 重新绘制
void handle_redraw(struct TextBox *textBox) {
    // 只刷新一行
    box_fill8(textBox->sheet->buffer, textBox->sheet->width, textBox->x0, textBox->current_line_y,
              textBox->x1,
              textBox->current_line_y + 16, COL8_000000);
    textBox->line_buffer[textBox->key_cursor_x] = '\0';
    print_str(textBox->sheet->buffer, textBox->sheet->width, textBox->x0, textBox->current_line_y,
              textBox->line_buffer, COLOR_WHITE);
    draw_8_16_block(textBox->sheet, textBox->x0 + textBox->key_cursor_x * 8, textBox->current_line_y,
                    COLOR_WHITE);
    refresh_sheet(textBox->sheet);
}

// 绘制一个边框
static void make_textbox8(struct Sheet *sht, int x0, int y0, int x1, int y1, int c) {
    box_fill8(sht->buffer, sht->width, x0 - 2, y0 - 3, x1 + 1, y0 - 3, COL8_848484);
    box_fill8(sht->buffer, sht->width, x0 - 3, y0 - 3, x0 - 3, y1 + 1, COL8_848484);
    box_fill8(sht->buffer, sht->width, x0 - 3, y1 + 2, x1 + 1, y1 + 2, COL8_FFFFFF);
    box_fill8(sht->buffer, sht->width, x1 + 2, y0 - 3, x1 + 2, y1 + 2, COL8_FFFFFF);
    box_fill8(sht->buffer, sht->width, x0 - 1, y0 - 2, x1 + 0, y0 - 2, COL8_000000);
    box_fill8(sht->buffer, sht->width, x0 - 2, y0 - 2, x0 - 2, y1 + 0, COL8_000000);
    box_fill8(sht->buffer, sht->width, x0 - 2, y1 + 1, x1 + 0, y1 + 1, COL8_C6C6C6);
    box_fill8(sht->buffer, sht->width, x1 + 1, y0 - 2, x1 + 1, y1 + 1, COL8_C6C6C6);
    box_fill8(sht->buffer, sht->width, x0 - 1, y0 - 1, x1 + 0, y1 + 0, c);
}

struct TextBox *newTextBox(struct Sheet *sheet, short x0, short y0, short x1, short y1) {
    struct TextBox *textBox = (struct TextBox *) memory_alloc(global_memory_manager, sizeof(struct TextBox));
    textBox->sheet = sheet;
    textBox->x0 = x0;
    textBox->y0 = y0;
    textBox->x1 = x1;
    textBox->y1 = y1;
    // (x1 - x0) / 8 - 1 因为最后有一个白色小方块，所以要 -1
    textBox->max_char_count_of_line = (x1 - x0) / 8 - 1;
    textBox->line_buffer = (char *) memory_alloc(global_memory_manager,
                                                 sizeof(char) * (textBox->max_char_count_of_line + 1));
    int i;
    for (i = 0; i <= textBox->max_char_count_of_line; ++i) {
        textBox->line_buffer[i] = '\0';
    }
    textBox->key_cursor_x = 0;   // 当前键盘光标位置
    textBox->current_line_y = textBox->y0;
    make_textbox8(sheet, textBox->x0, textBox->y0, textBox->x1, textBox->y1,
                  COL8_000000);
    refresh_sheet(sheet);
    return textBox;
}