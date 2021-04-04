//
// Created by liwei1 on 2021/4/4.
//

#ifndef WRITEOS_MOUSEDECODER_H
#define WRITEOS_MOUSEDECODER_H

struct MouseDecoder {
    int status;
    unsigned char buffer[3];
};

// 接收一个数据，如果好了会返回非空值，否则返回 nullptr
unsigned char *recv_data(struct MouseDecoder *m, unsigned char datum);

// 初始化 mouse
void init_mouse(struct MouseDecoder *m);

#endif //WRITEOS_MOUSEDECODER_H
