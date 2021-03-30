//
// Created by liwei1 on 2021/3/27.
//
#include <stdio.h>
#include "bootpack.h"

struct BootInfo *const Boot_Info_Ptr = (struct BootInfo *const) 0x0ff0;


void HariMain(void) {
    init_palette();
    set_white_background();
    char s[40] = {};
    char mouse[16 * 16] = {};
    sprintf(s, "screenX = %d", Boot_Info_Ptr->screenX);
    box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, COL8_FF0000, 20, 20, 120, 120);
    box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, COL8_00FF00, 70, 50, 170, 150);
    box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, COL8_0000FF, 120, 80, 220, 180);
    print_str(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 8, 8, s, COLOR_BLACK);
    init_mouse_cursor8(mouse, COLOR_WHITE);
    print_mouse(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 32, 32, 16, 16, mouse);
    init_gdt();
    init_idt();
    run();
}


void run() {
    while (1) {
        io_hlt();
    }
}