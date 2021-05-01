//
// Created by liwei1 on 2021/4/3.
//

#ifndef WRITEOS_NASKFUNC_H
#define WRITEOS_NASKFUNC_H

#include "global.h"

void io_hlt(void);

// 禁用中断
void io_cli(void);

// 处理中断
void io_sti();

void io_stihlt();

void io_out8(int port, int data);

int io_in8(int port);

int io_load_eflags(void);

void io_store_eflags(int eflags);


// 定时器的中断处理函数
void asm_int_handler20();

// 键盘的中断处理函数
void asm_int_handler21();

// 鼠标中断处理函数
void asm_int_handler2c();

//
void asm_int_handler27();

// 段描述符
void load_gdtr(int limit, int addr);

// 中段向量
void load_idtr(int limit, int addr);

int load_cr0(void);

void store_cr0(int cr0);

bool asm_memory_is_valid(unsigned int *pMemory);

void load_tr(int tr);

void jmp_far(int eip,int cs);

#endif //WRITEOS_NASKFUNC_H
