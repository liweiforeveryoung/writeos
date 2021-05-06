#include "naskfunc.h"
#include "dsctbl.h"

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

// 初始化 global descriptor table（段表）
void init_gdt() {
    // gdt 的地址
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
    // init all segment descriptors
    int i;
    for (i = 0; i < 8192; ++i) {
        set_segment_desc(gdt + i, 0, 0, 0);
    }
    load_gdtr(LIMIT_GDT, (int) gdt);
    set_segment_desc(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
    set_segment_desc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);
    load_gdtr(LIMIT_GDT, (int) gdt);
}

// 代码段的起始编号，这个起始编号可以是任意值（当然这个任意值不能超过段的大小），只要不影响到别人就好（例如之前的 tss 信息）
const int code_segment_begin_index = 1003;

// 代码段
void set_code_desc(int codeNo, int addr, unsigned int limit) {
    // gdt 的地址
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
    set_segment_desc(gdt + code_segment_begin_index + codeNo, limit, addr, AR_CODE32_ER);
}

// 跳到代码段，codeNo 是代码段的 id
void jmp_to_code_segment(int codeNo) {
    jmp_far(0, (code_segment_begin_index + codeNo) * 8);
}

void set_tss_desc(int tssNo, int tssAddr) {
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
    set_segment_desc(gdt + tssNo, 103, tssAddr, AR_TSS32);
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