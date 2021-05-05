//
// Created by liwei1 on 2021/5/5.
//

#ifndef WRITEOS_TEXTBOX_H
#define WRITEOS_TEXTBOX_H

struct TextBox {
    struct Sheet *sheet;
    short x0;
    short y0;
    short x1;
    short y1;
    short key_cursor_x;   // 当前键盘光标位置
    short max_char_count_of_line;   // 每行的最大输入字符
    short current_line_y; // 当前输入的行的 y 坐标
    char *line_buffer;      // 行缓冲区
};

// 处理 enter 键
void handle_enter(struct TextBox *textBox);

void handle_redraw(struct TextBox *textBox);

// 处理 backspace 键盘
void handle_backspace(struct TextBox *textBox);

// 处理新的字符输入
void handle_new_char_come(struct TextBox *textBox, char newChar);

// 更新一行字符串
void handle_new_line(struct TextBox *textBox, char *line);

struct TextBox *newTextBox(struct Sheet *sheet, short x0, short y0, short x1, short y1);

#endif //WRITEOS_TEXTBOX_H
