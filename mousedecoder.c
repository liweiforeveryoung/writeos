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
            m->x = datum;
            if ((m->flag & 0x10) != 0) {
                m->x |= 0xff00;
            }
            m->status = 3;
            break;
        case 3:
            m->y = datum;  // 标与屏幕的y方向正好相反
            if ((m->flag & 0x20) != 0) {
                m->y |= 0xff00;
            }
            m->y = -(m->y);
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

short get_mouse_x(struct MouseDecoder *m) {
    return m->x;
}

short get_mouse_y(struct MouseDecoder *m) {
    return m->y;
}