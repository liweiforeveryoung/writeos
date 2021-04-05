//
// Created by liwei1 on 2021/4/4.
//

#include "mousedecoder.h"

bool recv_data(struct MouseDecoder *m, unsigned char datum) {
    // 见 readme
    switch (m->status) {
        case 0:
            if (datum == 0xfa) {
                m->status = 1;
            }
            break;
        case 1:
            m->flag = datum;
            m->status = 2;
            break;
        case 2:
            if ((m->flag & 0x10) != 0) {
                datum |= 0xffffff00;    // todo 应该用 int 还是用 unsigned char
            }
            m->x = datum;
            m->status = 3;
            break;
        case 3:
            if ((m->flag & 0x20) != 0) {
                datum |= 0xffffff00;    // todo 应该用 int 还是用 unsigned char
            }
            m->y = -datum;  // 标与屏幕的y方向正好相反
            m->status = 1;
            return true;
        default:
            break;
    }
    return false;
}

void init_mouse(struct MouseDecoder *m) {
    m->status = 0;
}

enum Button get_button(struct MouseDecoder *m) {
    return m->flag & 0x07;
}

unsigned char get_mouse_x(struct MouseDecoder *m) {
    return m->x;
}

unsigned char get_mouse_y(struct MouseDecoder *m) {
    return m->y;
}