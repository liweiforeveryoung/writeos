//
// Created by liwei1 on 2021/5/2.
//

#include "task.h"
#include "naskfunc.h"

bool switch_task_task4 = false;

void switch_task() {
    switch_task_task4 = !switch_task_task4;
    if (switch_task_task4) {
        jmp_far(0, 4 * 8);
    }else{
        jmp_far(0, 3 * 8);
    }
}