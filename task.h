//
// Created by liwei1 on 2021/5/2.
//

#ifndef WRITEOS_TASK_H
#define WRITEOS_TASK_H
// 多任务相关
struct Tss32 {
    int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
    int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    int es, cs, ss, ds, fs, gs;
    int ldtr, iomap;
};

struct Task {
    int sel; // sel用来存放GDT的编号
    // int flags;
    struct Tss32 tss;
};

struct TaskController {
    int runningTasksCount; // 正在运行的任务数
    int currentTaskIndex; // 当前正在运行的是哪个任务
    struct Task tasks[100];
};

extern struct TaskController *GlobalTaskController;

void AddTask(int taskAddr);

void InitGlobalTaskController();

// 切换任务
void switch_task();

#endif //WRITEOS_TASK_H
