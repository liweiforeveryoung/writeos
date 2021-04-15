//
// Created by liwei1 on 2021/4/3.
//

#ifndef WRITEOS_KEYBUFFER_H
#define WRITEOS_KEYBUFFER_H

#include "global.h"

#define KeyBufferLength 200

enum DataType {
    FromKeyBoard = 0, FromMouse, FromTimer
};

// 这是一个 buffer ring 的实现，只存 length - 1 个数据
// 当满了的话，插入数据将失败
// 当空了的话，读取数据将失败
struct KeyBuffer {
    unsigned char data[KeyBufferLength];
    unsigned char type[KeyBufferLength];
    int cur_index;  // 当前应该被读取的 index
    int end_index;  // 最后一个有效 index 的下一个 index
};

void init_key_buffer(struct KeyBuffer *buffer);

// 判斷 buffer 是否滿了
bool buffer_is_full(struct KeyBuffer *buffer);

// buffer 是否是空的
bool buffer_is_empty(struct KeyBuffer *buffer);

// 成功则返回 true，失败则返回 false
bool write_data_into_buffer(struct KeyBuffer *buffer, unsigned char datum, unsigned char type);

bool read_data_from_buffer(struct KeyBuffer *buffer, unsigned char *datum, unsigned char *type);


#endif //WRITEOS_KEYBUFFER_H
