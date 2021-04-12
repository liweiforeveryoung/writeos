//
// Created by liwei1 on 2021/3/27.
//
#include <stdio.h>
#include "bootpack.h"
#include "keybuffer.h"
#include "naskfunc.h"
#include "graphic.h"
#include "dsctbl.h"
#include "int.h"
#include "kbc.h"
#include "mousedecoder.h"
#include "memorymanager.h"
#include "sheet.h"

unsigned int mem_test(unsigned int start, unsigned int end);

struct BootInfo *const Boot_Info_Ptr = (struct BootInfo *const) 0x00000ff0;

struct KeyBuffer Key_buffer;

const short MouseWidth = 16;
const short MouseHeight = 16;
char mouse[16 * 16];

#define MEMORY_MANAGER_ADDR            0x003c0000

void init_manager(struct MemoryManager *manager);

void HariMain(void) {
    init_gdt();
    init_idt();
    init_pic();
    init_key_buffer(&Key_buffer);
    io_sti();       // todo 为什么把 sti 放在这里可以达到效果，明明 在 init_palette 里调用了 io_cli，按理按照 cli 之后就不该会鼠标有反应了
    init_palette();
    init_manager(global_memory_manager);
    char s[40] = {};
    sprintf(s, "screenX = %d", Boot_Info_Ptr->screenX);
    struct SheetControl *sheet_control = new_sheet_control(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX,
                                                           Boot_Info_Ptr->screenY);

    struct Sheet *root_sheet = create_sheet(sheet_control);
    init_sheet(root_sheet, 0, 0, Boot_Info_Ptr->screenX, Boot_Info_Ptr->screenY, COLOR_WHITE);

    struct Sheet *sheet_2 = create_sheet(sheet_control);
    init_sheet(sheet_2, 20, 20, 100, 100, COL8_FF0000);
    sheet_control_draw(sheet_control);
    init_mouse_cursor8(mouse, COLOR_WHITE);
    struct Sheet *mouse_sheet = create_sheet(sheet_control);
    mouse_sheet->color = COLOR_BLACK;
    // print_mouse(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 32, 32, MouseWidth, MouseHeight, mouse);
    mouse_sheet->width = MouseWidth;
    mouse_sheet->height = MouseHeight;
    // sprintf(s, "memory %d mb,free: %dkb", total_memory / (1024 * 1024), memory_total(global_memory_manager) / 1024);
    // print_str(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 64, 64, s, COLOR_BLACK);
    init_keyboard();
    enable_mouse();
    run(mouse_sheet, sheet_control);
}

void init_manager(struct MemoryManager *manager) {
    manager_init(global_memory_manager);
    const unsigned int memory_begin_addr = 0x00400000;
    unsigned int total_memory = mem_test(memory_begin_addr, 0xbfffffff);
    memory_free(global_memory_manager, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009e000 */
    memory_free(global_memory_manager, memory_begin_addr, total_memory - memory_begin_addr);
}


void run(struct Sheet *mouse_sheet, struct SheetControl *control) {
    unsigned char input, type;
    bool mouse_is_ready;
    short mouse_x, mouse_y;
    short x, y;
    x = Boot_Info_Ptr->screenX / 2;
    y = Boot_Info_Ptr->screenY / 2;
    bool exist = false;
    struct MouseDecoder mouse_decoder;
    enum Button button;
    init_mouse(&mouse_decoder);
    while (1) {
        io_cli();       // 禁用中断
        exist = read_data_from_buffer(&Key_buffer, &input, &type);
        if (exist) {
            io_sti();
            char str[10] = {0};
            // 如果 buffer 内有数据
            switch (type) {
                case FromMouse:
                    mouse_is_ready = recv_data(&mouse_decoder, input);
                    if (mouse_is_ready) {
                        mouse_x = get_mouse_x(&mouse_decoder);
                        mouse_y = get_mouse_y(&mouse_decoder);
                        x += mouse_x;
                        y += mouse_y;

                        x = min_short(x, Boot_Info_Ptr->screenX - MouseWidth);
                        x = max_short(x, 0);
                        y = min_short(y, Boot_Info_Ptr->screenY - MouseHeight);
                        y = max_short(y, 0);

                        button = get_button(&mouse_decoder);
                        mouse_sheet->x0 = x;
                        mouse_sheet->y0 = y;
                        sheet_control_draw(control);
                        // print_mouse(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, x, y, MouseWidth,
                        //             MouseHeight, mouse);
                    }

                    break;
                case FromKeyBoard:
                    // sprintf(str, "k %x", input);
                    // box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 20, 20, 200, 200, COLOR_WHITE);
                    // print_str(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 20, 20, str, COLOR_BLACK);
                    break;
                default:
            }
        } else {
            // 如果 buffer 内没有数据 就继续睡觉
            io_stihlt();    // 原因见 readme
        }
    }
}

#define E_FLAGS_AC_BIT      0x00040000
#define CR0_CACHE_DISABLE   0x60000000

unsigned int mem_test_sub(unsigned int start, unsigned int end);

bool memory_is_valid(unsigned int *pMemory);

unsigned int mem_test(unsigned int start, unsigned int end) {
    char flg486 = 0;
    unsigned int e_flag, cr0, i;
    // 确认 cpu 是 386 还是 486 以上的
    e_flag = io_load_eflags();
    e_flag |= E_FLAGS_AC_BIT;       // ac-bit = 1
    io_store_eflags(e_flag);
    e_flag = io_load_eflags();
    if ((e_flag & E_FLAGS_AC_BIT) != 0) {
        // 如果是 386，即使设定 ac = 1，ac的值还是会自动回到 0
        flg486 = 1;
    }
    e_flag &= ~E_FLAGS_AC_BIT;  // ac-bit = 0
    io_store_eflags(e_flag);

    // cpu 386 没有 cache
    if (flg486 != 0) {
        cr0 = load_cr0();
        cr0 |= CR0_CACHE_DISABLE;   // 禁止缓存
        store_cr0(cr0);
    }
    i = mem_test_sub(start, end);
    if (flg486 != 0) {
        cr0 = load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE;   // 禁止缓存
        store_cr0(cr0);
    }
    return i;
}

unsigned int mem_test_sub(unsigned int start, unsigned int end) {
    unsigned int i, *p, old;
    for (i = start; i <= end; i += 0x1000) {
        p = (unsigned int *) (i + 0xffc);   // 读取末尾的 1kb
        old = *p;
        if (!asm_memory_is_valid(p)) {
            break;
        }
        *p = old;
    }
    return i;
}

bool memory_is_valid(unsigned int *pMemory) {
    unsigned int pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
    *pMemory = pat0;
    *pMemory ^= 0xffffffff;
    if (*pMemory != pat1) {
        return false;
    }
    *pMemory ^= 0xffffffff;
    if (*pMemory != pat0) {
        return false;
    }
    return true;
}