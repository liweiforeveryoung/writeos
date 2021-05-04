//
// Created by liwei1 on 2021/4/3.
//
#include "signalbuffer.h"
#include "memorymanager.h"

// 成功则返回 true，失败则返回 false
bool write_data_into_buffer(struct SignalBuffer *buffer, unsigned char datum, unsigned char type);
// 一个辅助小函数
static int next_index(int length, int cur_index) {
    return ++cur_index % length;
}

// 判斷 buffer 是否滿了
bool buffer_is_full(struct SignalBuffer *buffer) {
    return next_index(SignalBufferLength, buffer->end_index) == buffer->cur_index;
}

// buffer 是否是空的
bool buffer_is_empty(struct SignalBuffer *buffer) {
    return buffer->cur_index == buffer->end_index;
}

struct SignalBufferController {
    struct SignalBuffer *buffers[100];
    int count;
};

struct SignalBufferController *SignalBufferController;

void initSignalBufferController() {
    SignalBufferController = (struct SignalBufferController *) memory_alloc(global_memory_manager,
                                                                            sizeof(struct SignalBufferController));
}


void signal_comes(unsigned char datum, unsigned char type) {
    int i;
    for (i = 0; i < SignalBufferController->count; i++) {
        write_data_into_buffer(SignalBufferController->buffers[i], datum, type);
    }
}

// 订阅信号
struct SignalBuffer *order_signal() {
    // todo 屏蔽信号
    struct SignalBuffer *buffer = (struct SignalBuffer *) memory_alloc(global_memory_manager,
                                                                       sizeof(struct SignalBuffer));
    buffer->cur_index = 0;
    buffer->end_index = 0;
    SignalBufferController->buffers[SignalBufferController->count] = buffer;
    SignalBufferController->count++;
    return buffer;
}

// 成功则返回 true，失败则返回 false
bool write_data_into_buffer(struct SignalBuffer *buffer, unsigned char datum, unsigned char type) {
    if (buffer_is_full(buffer)) {
        return false;
    }
    buffer->data[buffer->end_index] = datum;
    buffer->type[buffer->end_index] = type;
    buffer->end_index = next_index(SignalBufferLength, buffer->end_index);
    return true;
}

bool read_data_from_buffer(struct SignalBuffer *buffer, unsigned char *datum, unsigned char *type) {
    if (buffer_is_empty(buffer)) {
        return false;
    }
    *datum = buffer->data[buffer->cur_index];
    *type = buffer->type[buffer->cur_index];
    buffer->cur_index = next_index(SignalBufferLength, buffer->cur_index);
    return true;
}

