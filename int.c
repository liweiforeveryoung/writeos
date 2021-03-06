//
// Created by liwei1 on 2021/4/1.
//
#include "signalbuffer.h"
#include "naskfunc.h"
#include "task.h"

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

const int PORT_KEYDAT = 0x0060;

void init_pic(void) {
    io_out8(PIC0_IMR, 0xff); // 禁止所有中断
    io_out8(PIC1_IMR, 0xff); // 禁止所有中断

    io_out8(PIC0_ICW1, 0x11);// 边沿触发模式(edge trigger mode)   电气特性 照抄即可
    io_out8(PIC0_ICW2, 0x20); // IRQ0~7 由 int20~27 接收
    io_out8(PIC0_ICW3, 1 << 2);  // PIC1 由 IRQ2 连接
    io_out8(PIC0_ICW4, 0x01); // 无缓冲区模式 电气特性 照抄即可

    io_out8(PIC1_ICW1, 0x11);// 边沿触发模式(edge trigger mode) 电气特性 照抄即可
    io_out8(PIC1_ICW2, 0x28); // IRQ8~15 由 int28~2f 接收
    io_out8(PIC1_ICW3, 1 << 2);  // PIC1 由 IRQ2 连接
    io_out8(PIC1_ICW4, 0x01); // 无缓冲区模式 电气特性 照抄即可

    io_out8(PIC0_IMR, 0xf8);    // oxf9  = 1111 1000 如果等于 1，代表屏蔽该中断 mask，处理 1号中断2号中断
    io_out8(PIC1_IMR, 0xef);    // oxef  = 1110 1111 处理 12(c) 号中断
}

// 鼠标是IRQ12，键盘是IRQ1
// 所以鼠标对应 int 2c
// 键盘对应    int 21

// handler keyboard event
void int_handler21(int *esp) {
    io_out8(PIC0_OCW2, 0x61);
    // 这个地方切忌要用 unsigned char 来接收，用 char 的话打印出来的东西会有点奇怪
    unsigned char input = io_in8(PORT_KEYDAT); // 想要处理下一次键盘中断有两个条件：1、io_out8 ocw 2、通过 io_in 把数据从端口中读出来；两者缺一不可
    signal_comes(input, FromKeyBoard);
}

// handler mouse event
void int_handler2c(int *esp) {
    io_out8(PIC1_OCW2, 0x64);   // 通知从 pic  4 号 irq 受理完成（从 pic 的 4号相当于 12 号）
    io_out8(PIC0_OCW2, 0x62);   // 通知主 pic 2 号受理完成，因为从 pic 接在主 pic 的 2号引线上
    unsigned char mouseInput = io_in8(PORT_KEYDAT);
    signal_comes(mouseInput, FromMouse);
}

// 来自PIC0的不完全中断对策,由于芯片组的原因，在PIC初始化时，Athlon 64X2机等会发生一次中断
// 该中断处理函数对该中断什么也不做就这样过去,为什么什么都不做？
// 该中断是由于PIC初始化时的电气噪声造成的,不必认真处理什么。
// 我试了一下，即使没有这个 handler 27 也不会有什么影响，但为了保险还是加上吧
// 免得之后出了什么幺蛾子
void int_handler27(int *esp) {
    io_out8(PIC0_OCW2, 0x67);
}


// 来自定时器的中断
void int_handler20(int *esp) {
    io_out8(PIC0_OCW2, 0x60);   // 通知主 pic 0 号受理完成
    signal_comes(0, FromTimer);
    switch_task();
}

#define PIT_CTRL    0x0043
#define PIT_CNT0    0x0040

// 初始化 pit，programmable interrupt timer
void init_pit(void) {
    io_out8(PIT_CTRL, 0x34);
    io_out8(PIT_CNT0, 0x9c);
    io_out8(PIT_CNT0, 0x2e);
}
