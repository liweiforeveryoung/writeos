//
// Created by liwei1 on 2021/5/2.
//

#include "task.h"
#include "naskfunc.h"
#include "memorymanager.h"
#include "dsctbl.h"

void switch_task() {
    if (GlobalTaskController->runningTasksCount < 2) {
        return;
    }
    struct Task *currentTask = GlobalTaskController->tasks + GlobalTaskController->currentTaskIndex;
    if (currentTask->timeHasRun < currentTask->priority) {
        currentTask->timeHasRun++;
        return;
    }
    currentTask->timeHasRun = 0;
    int next = (GlobalTaskController->currentTaskIndex + 1) % GlobalTaskController->runningTasksCount;
    int nextSel = GlobalTaskController->tasks[next].sel;
    GlobalTaskController->currentTaskIndex = next;
    jmp_far(0, nextSel);
}

struct TaskController *GlobalTaskController;

void InitGlobalTaskController() {
    GlobalTaskController = (struct TaskController *) memory_alloc(global_memory_manager, sizeof(struct TaskController));
    GlobalTaskController->tasks[0].tss.ldtr = 0;
    GlobalTaskController->tasks[0].tss.iomap = 0x40000000;
    GlobalTaskController->tasks[0].sel = 3 * 8;     // 起步為 3
    GlobalTaskController->tasks[0].priority = 2;    // 运行 0.2 秒
    GlobalTaskController->tasks[0].timeHasRun = 0;
    GlobalTaskController->runningTasksCount = 1;
    GlobalTaskController->currentTaskIndex = 0;
    int i = 0;
    for (; i < 100; i++) {
        set_tss_desc(3 + i, (int) &(GlobalTaskController->tasks[i].tss));
    }
    load_tr(3 * 8);     // 当前执行的是第三号任务，在执行这句代码之前，一定要先初始化第三号 tss
}

struct TaskController *GlobalTaskController;


void AddTask(int taskAddr, int priority) {
    struct Task *task = GlobalTaskController->tasks + GlobalTaskController->runningTasksCount;
    task->tss.ldtr = 0;
    task->tss.iomap = 0x40000000;
    task->tss.eip = taskAddr;
    task->tss.eflags = 0x00000202; /* IF = 1; */
    task->tss.eax = 0;
    task->tss.ecx = 0;
    task->tss.edx = 0;
    task->tss.ebx = 0;
    task->tss.esp = memory_alloc(global_memory_manager, 64 * 1024) + 64 * 1024; // 给任务 b 准备 64kb 的栈空间
    task->tss.ebp = 0;
    task->tss.esi = 0;
    task->tss.edi = 0;
    task->tss.es = 1 * 8;
    task->tss.cs = 2 * 8;   // GDT 的二号
    task->tss.ss = 1 * 8;   // GDT 的一号
    task->tss.ds = 1 * 8;   // GDT 的一号
    task->tss.fs = 1 * 8;   // GDT 的一号
    task->tss.gs = 1 * 8;   // GDT 的一号
    task->sel = (3 + GlobalTaskController->runningTasksCount) * 8;
    task->priority = priority;
    task->timeHasRun = 0;
    GlobalTaskController->runningTasksCount++;
}