//
// Created by liwei1 on 2021/4/4.
//

#ifndef WRITEOS_MOUSEDECODER_H
#define WRITEOS_MOUSEDECODER_H

#include "global.h"

struct MouseDecoder {
    unsigned char status;
    unsigned char flag;
    unsigned char x;
    unsigned char y;
};

// 接收一个数据，如果好了会返回 true,否则返回 false
bool recv_data(struct MouseDecoder *m, unsigned char datum);

// 初始化 mouse
void init_mouse(struct MouseDecoder *m);

enum Button {
    None = 0,
    Left = 1,
    Right = 2,
    Control = 4
};

enum Button get_button(struct MouseDecoder *m);

unsigned char get_mouse_x(struct MouseDecoder *m);

unsigned char get_mouse_y(struct MouseDecoder *m);

#endif //WRITEOS_MOUSEDECODER_H
