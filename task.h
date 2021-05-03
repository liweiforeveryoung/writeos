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
    int priority;   // 优先级，值为 1 到 10，分别代表运行 0.01 秒 或者运行 0.1 秒
    struct Tss32 tss;
};

struct TaskController {
    int runningTasksCount; // 正在运行的任务数
    int currentTaskIndex; // 当前正在运行的是哪个任务
    struct Task tasks[100];
};

extern struct TaskController *GlobalTaskController;

// 添加任务，taskAddr 为任务的地址，priority 为任务的优先级，值为 1 到 10，分别代表执行 0.01 秒或者 0.1 秒
void AddTask(int taskAddr, int priority);

void InitGlobalTaskController();

// 切换任务
void switch_task();

#endif //WRITEOS_TASK_H
