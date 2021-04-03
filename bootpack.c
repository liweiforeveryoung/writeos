//
// Created by liwei1 on 2021/3/27.
//
#include <stdio.h>
#include "bootpack.h"
#include "keybuffer.h"
#include "naskfunc.h"

struct BootInfo *const Boot_Info_Ptr = (struct BootInfo *const) 0x0ff0;

struct KeyBuffer Key_buffer;

void HariMain(void) {
    init_gdt();
    init_idt();
    init_pic();
    init_key_buffer(&Key_buffer);
    io_sti();       // todo 为什么把 sti 放在这里可以达到效果，明明 在 init_palette 里调用了 io_cli，按理按照 cli 之后就不该会鼠标有反应了
    init_palette();
    set_white_background();
    char s[40] = {};
    char mouse[16 * 16] = {};
    sprintf(s, "screenX = %d", Boot_Info_Ptr->screenX);
    box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 20, 20, 120, 120, COL8_FF0000);
    box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 70, 50, 170, 150, COL8_00FF00);
    box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 120, 80, 220, 180, COL8_0000FF);
    print_str(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 8, 8, s, COLOR_BLACK);
    init_mouse_cursor8(mouse, COLOR_WHITE);
    print_mouse(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 32, 32, 16, 16, mouse);

    run();
}


void run() {
    unsigned char input = 0;
    bool exist = false;
    while (1) {
        io_cli();       // 禁用中断
        exist = read_data_from_buffer(&Key_buffer, &input);
        if (exist) {
            io_sti();
            // 如果 buffer 内有数据
            char str[4] = {0};
            sprintf(str, "%x", input);
            box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 20, 20, 200, 200, COLOR_WHITE);
            print_str(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 20, 20, str, COLOR_BLACK);
        } else {
            // 如果 buffer 内没有数据 就继续睡觉
            io_stihlt();    // 原因见 readme
        }
    }
}