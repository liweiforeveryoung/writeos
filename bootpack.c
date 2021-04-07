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

unsigned int mem_test(unsigned int start, unsigned int end);

struct BootInfo *const Boot_Info_Ptr = (struct BootInfo *const) 0x00000ff0;

struct KeyBuffer Key_buffer;

const short MouseWidth = 16;
const short MouseHeight = 16;
char mouse[16 * 16];

void HariMain(void) {
    init_gdt();
    init_idt();
    init_pic();
    init_key_buffer(&Key_buffer);
    io_sti();       // todo 为什么把 sti 放在这里可以达到效果，明明 在 init_palette 里调用了 io_cli，按理按照 cli 之后就不该会鼠标有反应了
    init_palette();
    set_white_background();
    char s[40] = {};
    sprintf(s, "screenX = %d", Boot_Info_Ptr->screenX);
    box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 20, 20, 120, 120, COL8_FF0000);
    box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 70, 50, 170, 150, COL8_00FF00);
    box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 120, 80, 220, 180, COL8_0000FF);
    print_str(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 8, 8, s, COLOR_BLACK);
    init_mouse_cursor8(mouse, COLOR_WHITE);
    print_mouse(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 32, 32, MouseWidth, MouseHeight, mouse);

    unsigned int i;
    i = mem_test(0x00400000, 0xbfffffff) / (1024 * 1024);
    sprintf(s, "memory %dMB", i);
    print_str(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 64, 64, s, COLOR_BLACK);
    init_keyboard();
    enable_mouse();
    run();
}


void run() {
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
                        set_white_background();
                        print_mouse(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, x, y, MouseWidth,
                                    MouseHeight, mouse);
                    }

                    break;
                case FromKeyBoard:
                    sprintf(str, "k %x", input);
                    box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 20, 20, 200, 200, COLOR_WHITE);
                    print_str(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 20, 20, str, COLOR_BLACK);
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