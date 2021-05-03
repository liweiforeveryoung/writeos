#include "naskfunc.h"
#include "dsctbl.h"
#include "memorymanager.h"
#include "task.h"

void set_segment_desc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar) {
    if (limit > 0xfffff) {
        ar |= 0x8000; /* G_bit = 1 */
        limit /= 0x1000;
    }
    sd->limit_low = limit & 0xffff;
    sd->base_low = base & 0xffff;
    sd->base_mid = (base >> 16) & 0xff;
    sd->access_right = ar & 0xff;
    sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
    sd->base_high = (base >> 24) & 0xff;
}

const int AR_DATA32_RW = 0x4092;
const int AR_CODE32_ER = 0x409a;
const int LIMIT_BOTPAK = 0x0007ffff;
const int ADR_BOTPAK = 0x00280000;
const int LIMIT_GDT = 0x0000ffff;
const int ADR_GDT = 0x00270000;
const int AR_TSS32 = 0x0089;

struct TSS32 tss_a, tss_b;

void init_tss(int task_b_main_addr) {
    tss_a.ldtr = 0;
    tss_a.iomap = 0x40000000;
    load_tr(3 * 8);     // 当前执行的是第三号任务
    tss_b.ldtr = 0;
    tss_b.iomap = 0x40000000;
    tss_b.eip = task_b_main_addr;
    tss_b.eflags = 0x00000202; /* IF = 1; */
    tss_b.eax = 0;
    tss_b.ecx = 0;
    tss_b.edx = 0;
    tss_b.ebx = 0;
    tss_b.esp = memory_alloc(global_memory_manager, 64 * 1024) + 64 * 1024; // 给任务 b 准备 64kb 的栈空间
    tss_b.ebp = 0;
    tss_b.esi = 0;
    tss_b.edi = 0;
    tss_b.es = 1 * 8;
    tss_b.cs = 2 * 8;   // GDT 的二号
    tss_b.ss = 1 * 8;   // GDT 的一号
    tss_b.ds = 1 * 8;   // GDT 的一号
    tss_b.fs = 1 * 8;   // GDT 的一号
    tss_b.gs = 1 * 8;   // GDT 的一号
}


// 初始化 global descriptor table（段表）
void init_gdt() {
    // gdt 的地址
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
    // init all segment descriptors
    int i;
    for (i = 0; i < 8192; ++i) {
        set_segment_desc(gdt + i, 0, 0, 0);
    }
    set_segment_desc(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
    set_segment_desc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);
    set_segment_desc(gdt + 3, 103, (int) &tss_a, AR_TSS32);
    set_segment_desc(gdt + 4, 103, (int) &tss_b, AR_TSS32);
    load_gdtr(LIMIT_GDT, (int) gdt);
}

void set_interrupt_desc(struct INTERRUPT_DESCRIPTOR *id, int offset, int selector, int ar) {
    id->offset_low = offset & 0xffff;
    id->selector = selector;
    id->dw_count = (ar >> 8) & 0xff;
    id->access_right = ar & 0xff;
    id->offset_high = (offset >> 16) & 0xffff;
}


#define AR_INTGATE32    0x008e

// 初始化中断向量表  interrupt descriptor table
void init_idt() {
    struct INTERRUPT_DESCRIPTOR *idt = (struct INTERRUPT_DESCRIPTOR *) 0x0026f800;
    int i;
    for (i = 0; i < 256; i++) {
        set_interrupt_desc(idt + i, 0, 0, 0);
    }
    load_idtr(0x7ff, (int) idt);
    // 2 << 3 表示段号是 2，之所以要左移3是因为低三位有别的意思，这里的低三位必须为 0
    // 将该中断的属性，设定为0x008e。它表示这是用于中断处理的有效设定
    set_interrupt_desc(idt + 0x20, (int) asm_int_handler20, 2 * 8, AR_INTGATE32);
    set_interrupt_desc(idt + 0x21, (int) asm_int_handler21, 2 * 8, AR_INTGATE32);
    set_interrupt_desc(idt + 0x2c, (int) asm_int_handler2c, 2 * 8, AR_INTGATE32);
    set_interrupt_desc(idt + 0x27, (int) asm_int_handler27, 2 * 8, AR_INTGATE32);
}