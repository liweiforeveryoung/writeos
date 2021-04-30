//
// Created by liwei1 on 2021/4/30.
//

#include "timercb.h"

void timer_ctl_tick(struct TimerCallbackCtl *ctl) {
    int i = 0;
    for (; i < ctl->size; i++) {
        if (++ctl->cbs[i].curCount == ctl->cbs[i].dstCount) {
            ctl->cbs[i].cb();
            ctl->cbs[i].curCount = 0;
        }
    }
}

void timer_ctl_add(struct TimerCallbackCtl *ctl, int dstCount, void (*cb)(void)) {
    ctl->cbs[ctl->size].dstCount = dstCount;
    ctl->cbs[ctl->size].curCount = 0;
    ctl->cbs[ctl->size].cb = cb;
    ctl->size++;
}

struct TimerCallbackCtl GlobalTimerCallbackCtl;