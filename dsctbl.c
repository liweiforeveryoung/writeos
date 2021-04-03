#include <stdio.h>
#include "bootpack.h"

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

// 初始化 global descriptor table（段表）
void init_gdt() {
    // gdt 的地址
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;
    // init all segment descriptors
    int i;
    for (i = 0; i < 8192; ++i) {
        set_segment_desc(gdt + i, 0, 0, 0);
    }
    set_segment_desc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
    set_segment_desc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
    load_gdtr(0xffff, (int) gdt);
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
    set_interrupt_desc(idt + 0x21, (int) asm_int_handler21, 2 * 8, AR_INTGATE32);
    set_interrupt_desc(idt + 0x2c, (int) asm_int_handler2c, 2 * 8, AR_INTGATE32);
    set_interrupt_desc(idt + 0x27, (int) asm_int_handler27, 2 * 8, AR_INTGATE32);
}