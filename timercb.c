//
// Created by liwei1 on 2021/4/30.
//

#include "timercb.h"

void timer_ctl_tick(struct TimerCallbackCtl *ctl) {
    int i = 0;
    for (; i < ctl->size; i++) {
        if (ctl->cbs[i].isValid && ++ctl->cbs[i].curCount == ctl->cbs[i].dstCount) {
            if (ctl->cbs[i].isCronJob) {
                ctl->cbs[i].curCount = 0;
            } else {
                // 执行一次之后就不必再执行了
                ctl->cbs[i].isValid = false;
            }
            ctl->cbs[i].cb();
        }
    }
}

void timer_ctl_add(struct TimerCallbackCtl *ctl, int dstCount, void (*cb)(void), bool isCron) {
    ctl->cbs[ctl->size].dstCount = dstCount;
    ctl->cbs[ctl->size].curCount = 0;
    ctl->cbs[ctl->size].cb = cb;
    ctl->cbs[ctl->size].isCronJob = isCron;
    ctl->cbs[ctl->size].isValid = true;
    ctl->size++;
}

struct TimerCallbackCtl GlobalTimerCallbackCtl;