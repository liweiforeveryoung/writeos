//
// Created by liwei1 on 2021/5/2.
//

#ifndef WRITEOS_TASK_H
#define WRITEOS_TASK_H
// 多任务相关
struct TSS32 {
    int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
    int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    int es, cs, ss, ds, fs, gs;
    int ldtr, iomap;
};

struct Task {

};

struct TaskController {

};

void AddTask();

// 切换任务
void switch_task();
#endif //WRITEOS_TASK_H
