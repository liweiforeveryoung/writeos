//
// Created by liwei1 on 2021/4/3.
//

#ifndef WRITEOS_SIGNALBUFFER_H
#define WRITEOS_SIGNALBUFFER_H

#include "global.h"

#define SignalBufferLength 200

enum DataType {
    FromKeyBoard = 0, FromMouse, FromTimer
};

// 这是一个 buffer ring 的实现，只存 length - 1 个数据
// 当满了的话，插入数据将失败
// 当空了的话，读取数据将失败
struct SignalBuffer {
    unsigned char data[SignalBufferLength];
    unsigned char type[SignalBufferLength];
    int cur_index;  // 当前应该被读取的 index
    int end_index;  // 最后一个有效 index 的下一个 index
};

// 订阅信号
struct SignalBuffer *order_signal();
// 初始化
void initSignalBufferController();
// 判斷 buffer 是否滿了
bool buffer_is_full(struct SignalBuffer *buffer);

// buffer 是否是空的
bool buffer_is_empty(struct SignalBuffer *buffer);

// 信号来啦
void signal_comes(unsigned char datum, unsigned char type);

bool read_data_from_buffer(struct SignalBuffer *buffer, unsigned char *datum, unsigned char *type);


#endif //WRITEOS_SIGNALBUFFER_H
