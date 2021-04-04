//
// Created by liwei1 on 2021/4/4.
//

#include "mousedecoder.h"
#include "global.h"

unsigned char *recv_data(struct MouseDecoder *m, unsigned char datum) {
    // 见 readme
    switch (m->status) {
        case 0:
            if (datum == 0xfa) {
                m->status = 1;
            }
            break;
        case 1:
            m->buffer[0] = datum;
            m->status = 2;
            break;
        case 2:
            m->buffer[1] = datum;
            m->status = 3;
            break;
        case 3:
            m->buffer[2] = datum;
            m->status = 1;
            return m->buffer;
        default:
            break;
    }
    return nullptr;
}

void init_mouse(struct MouseDecoder *m) {
    m->status = 0;
}