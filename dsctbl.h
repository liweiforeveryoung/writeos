//
// Created by liwei1 on 2021/4/3.
//

#ifndef WRITEOS_DSCTBL_H
#define WRITEOS_DSCTBL_H

// 对于段的描述
struct SEGMENT_DESCRIPTOR {
    short limit_low;
    short base_low;     // 段的地址
    char base_mid;      // 段的地址
    char access_right;
    char limit_high;
    char base_high;     // 段的地址
};
// 为什么要分为3段呢？主要是为了与80286时代的程序兼容。有了这样的规格，80286用的操作系统，也可以不用修改就在386以后的CPU上运行了。

// 对于中断向量的描述
struct INTERRUPT_DESCRIPTOR {
    short offset_low;
    short selector;
    char dw_count;
    char access_right;
    short offset_high;
};

// 初始化 global descriptor table（段表）
void init_gdt();

// 初始化中断向量表  interrupt descriptor table
void init_idt();

void set_tss_desc(int tssNo, int tssAddr);

// 代码段
void set_code_desc(int codeNo, int addr, unsigned int limit);

// 跳到代码段，codeNo 是代码段的 id
void jmp_to_code_segment(int codeNo);

// 调用代码段，codeNo 是代码段的 id
void call_code_segment(int codeNo);
#endif //WRITEOS_DSCTBL_H
