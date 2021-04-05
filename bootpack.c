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