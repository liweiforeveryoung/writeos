//
// Created by liwei1 on 2021/4/3.
//
#include "keybuffer.h"

void init_key_buffer(struct KeyBuffer *buffer) {
    buffer->cur_index = 0;
    buffer->end_index = 0;
}

// 一个辅助小函数
static int next_index(int length, int cur_index) {
    return ++cur_index % length;
}

// 判斷 buffer 是否滿了
bool buffer_is_full(struct KeyBuffer *buffer) {
    return next_index(KeyBufferLength, buffer->end_index) == buffer->cur_index;
}

// buffer 是否是空的
bool buffer_is_empty(struct KeyBuffer *buffer) {
    return buffer->cur_index == buffer->end_index;
}

// 成功则返回 true，失败则返回 false
bool write_data_into_buffer(struct KeyBuffer *buffer, unsigned char datum, unsigned char type) {
    if (buffer_is_full(buffer)) {
        return false;
    }
    buffer->data[buffer->end_index] = datum;
    buffer->type[buffer->end_index] = type;
    buffer->end_index = next_index(KeyBufferLength, buffer->end_index);
    return true;
}

bool read_data_from_buffer(struct KeyBuffer *buffer, unsigned char *datum, unsigned char *type) {
    if (buffer_is_empty(buffer)) {
        return false;
    }
    *datum = buffer->data[buffer->cur_index];
    *type = buffer->type[buffer->cur_index];
    buffer->cur_index = next_index(KeyBufferLength, buffer->cur_index);
    return true;
}

