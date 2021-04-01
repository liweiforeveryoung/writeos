//
// Created by liwei1 on 2021/4/1.
//
#include "bootpack.h"

const int PIC0_ICW1 = 0x0020;
const int PIC0_OCW2 = 0x0020;
const int PIC0_IMR = 0x0021;
const int PIC0_ICW2 = 0x0021;
const int PIC0_ICW3 = 0x0021;
const int PIC0_ICW4 = 0x0021;
const int PIC1_ICW1 = 0x00a0;
const int PIC1_OCW2 = 0x00a0;
const int PIC1_IMR = 0x00a1;
const int PIC1_ICW2 = 0x00a1;
const int PIC1_ICW3 = 0x00a1;
const int PIC1_ICW4 = 0x00a1;

void init_pic(void) {
    io_out8(PIC0_IMR, 0xff); // 禁止所有中断
    io_out8(PIC1_IMR, 0xff); // 禁止所有中断

    io_out8(PIC0_ICW1, 0x11);// 边沿触发模式(edge trigger mode)   电气特性 照抄即可
    io_out8(PIC0_ICW2, 0x20); // IRQ0~7 由 int20~27 接收
    io_out8(PIC0_ICW3, 1 << 2);  // PIC1 由 IRQ2 连接
    io_out8(PIC0_ICW4, 0x01); // 无缓冲区模式 电气特性 照抄即可

    io_out8(PIC1_ICW1, 0x11);// 边沿触发模式(edge trigger mode) 电气特性 照抄即可
    io_out8(PIC0_ICW2, 0x28); // IRQ8~15 由 int28~2f 接收
    io_out8(PIC0_ICW3, 1 << 2);  // PIC1 由 IRQ2 连接
    io_out8(PIC1_ICW4, 0x01); // 无缓冲区模式 电气特性 照抄即可
}

