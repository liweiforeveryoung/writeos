//
// Created by liwei1 on 2021/4/30.
//

#ifndef WRITEOS_TIMERCB_H
#define WRITEOS_TIMERCB_H

#include "global.h"

// 定时器回调
struct TimerCallback {
    unsigned int dstCount; // 多长时间执行一次，单位为 0.01 s
    unsigned int curCount;
    bool isValid;          // 这个任务是否是有效的
    bool isCronJob;        // 是否是一个周期执行的任务
    void (*cb)(void);      // 到期时需要执行的动作
};

extern struct TimerCallbackCtl GlobalTimerCallbackCtl;

// 定时器回调管理器
struct TimerCallbackCtl {
    int size;
    struct TimerCallback cbs[100];
};

void timer_ctl_tick(struct TimerCallbackCtl *ctl);

// dstCount: 多长时间之后执行任务，单位为 0.01 s
// cb: 所想要执行的任务
// isCron: 是否是一个周期执行的任务
void timer_ctl_add(struct TimerCallbackCtl *ctl, int dstCount, void (*cb)(void), bool isCron);

#endif //WRITEOS_TIMERCB_H
