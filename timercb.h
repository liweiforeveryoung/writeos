//
// Created by liwei1 on 2021/4/30.
//

#ifndef WRITEOS_TIMERCB_H
#define WRITEOS_TIMERCB_H

// 定时器回调
struct TimerCallback {
    unsigned int curCount,dstCount;     // 多少秒执行一次
    void (*cb)(void);      // 到期时需要执行的动作
};

extern struct TimerCallbackCtl GlobalTimerCallbackCtl;

// 定时器回调管理器
struct TimerCallbackCtl {
    int size;
    struct TimerCallback cbs[100];
};

void timer_ctl_tick(struct TimerCallbackCtl *ctl);

void timer_ctl_add(struct TimerCallbackCtl *ctl, int dstCount, void (*cb)(void));

#endif //WRITEOS_TIMERCB_H
