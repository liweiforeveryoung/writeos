//
// Created by liwei1 on 2021/3/27.
//
#include <stdio.h>
#include "bootpack.h"
#include "signalbuffer.h"
#include "naskfunc.h"
#include "graphic.h"
#include "dsctbl.h"
#include "int.h"
#include "kbc.h"
#include "mousedecoder.h"
#include "memorymanager.h"
#include "sheet.h"
#include "timercb.h"

unsigned int mem_test(unsigned int start, unsigned int end);

void init_mouse_sheet(struct SheetControl *sheet_control);

void make_window8(char *buf, short xSize, short ySize, char *title);

struct BootInfo *const Boot_Info_Ptr = (struct BootInfo *const) 0x00000ff0;

struct SignalBuffer Signal_buffer;

const short MouseWidth = 16;
const short MouseHeight = 16;

#define MEMORY_MANAGER_ADDR            0x003c0000

void init_manager();

struct Sheet *mouse_sheet;

static char KeyTable[0x54] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0, 0,
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0, 0,
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0, 0, ']',
        'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
        '2', '3', '0', '.'
};


void task_b_main(void) {
    unsigned char input, type;
    bool exist;
    while (1) {
        io_cli();       // 禁用中断
        exist = read_data_from_buffer(&Signal_buffer, &input, &type);
        if (exist) {
            io_sti();
            if (type == FromTimer) {
                timer_ctl_tick(&GlobalTimerCallbackCtl);
            }
        } else {
            // 如果 buffer 内没有数据 就继续睡觉
            io_stihlt();    // 原因见 readme
        }
    }
}


void HariMain(void) {
    init_gdt();
    init_idt();
    init_pic();
    init_pit();
    init_signal_buffer(&Signal_buffer);
    io_sti();       // todo 为什么把 sti 放在这里可以达到效果，明明 在 init_palette 里调用了 io_cli，按理按照 cli 之后就不该会鼠标有反应了
    init_palette();
    init_manager();
    init_tss(task_b_main);
    char s[40] = {};
    sprintf(s, "screenX = %d", Boot_Info_Ptr->screenX);
    struct SheetControl *sheet_control = new_sheet_control(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX,
                                                           Boot_Info_Ptr->screenY);

    struct Sheet *root_sheet = create_sheet(sheet_control, 0, 0, Boot_Info_Ptr->screenX, Boot_Info_Ptr->screenY);
    set_sheet_color(root_sheet, COLOR_WHITE);

    struct Sheet *red_sheet = create_sheet(sheet_control, 20, 20, 100, 100);
    set_sheet_color(red_sheet, COL8_FF0000);

    struct Sheet *window = create_sheet(sheet_control, 20, 20, 160, 68);
    set_sheet_color(window, COLOR_TRANSPARENT);
    make_window8(window->buffer, window->width, window->height, "window");
    sheet_control_draw(sheet_control);

    struct Sheet *char_window = create_sheet(sheet_control, 30, 30, 160, 52);
    set_sheet_color(char_window, COLOR_TRANSPARENT);
    make_window8(char_window->buffer, char_window->width, char_window->height, "window");
    sheet_control_draw(sheet_control);

    init_mouse_sheet(sheet_control);
    init_keyboard();
    enable_mouse();

    // 五秒时候切换任务
    timer_ctl_add(&GlobalTimerCallbackCtl, 500, taskswitch4, false);
    timer_ctl_add(&GlobalTimerCallbackCtl, 1000, taskswitch3, false);

    run(char_window);
}

// 初始化鼠标图层
void init_mouse_sheet(struct SheetControl *sheet_control) {
    mouse_sheet = create_sheet(sheet_control, 0, 0, MouseWidth, MouseHeight);
    init_mouse_cursor8(mouse_sheet->buffer, COLOR_TRANSPARENT);
    // 故意把 mouse_sheet 往下移动一层测试一下
    move_down_sheet(mouse_sheet);
    move_up_sheet(mouse_sheet);
}


void init_manager() {
    manager_init(global_memory_manager);
    const unsigned int memory_begin_addr = 0x00400000;
    unsigned int total_memory = mem_test(memory_begin_addr, 0xbfffffff);
    memory_free(global_memory_manager, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009e000 */
    memory_free(global_memory_manager, memory_begin_addr, total_memory - memory_begin_addr);
}

void run(struct Sheet *window) {
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
    char buf[20] = {0};
    int key_cursor_x = 0;   // 当前键盘光标位置
    while (1) {
        io_cli();       // 禁用中断
        exist = read_data_from_buffer(&Signal_buffer, &input, &type);
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
                        set_sheet_pos(mouse_sheet, x, y);

                        // 前行拽着窗口移动
                        if (button == Left) {
                            set_sheet_pos(window, x - 80, y - 8);
                        }
                    }

                    break;
                case FromKeyBoard:
                    if (input < 0x54 || input == 0x0e) {
                        // input < 0x54 是为了只接收按下的字符，不接受松开的字符
                        if ((input < 0x54) && (KeyTable[input] != 0)) {
                            buf[key_cursor_x] = KeyTable[input];
                            ++key_cursor_x;
                            // 最多打印十个字符
                            if (key_cursor_x > 10) {
                                key_cursor_x = 10;
                            }
                        } else if (input == 0x0e) {
                            // 退格键
                            --key_cursor_x;
                            if (key_cursor_x < 0) {
                                key_cursor_x = 0;
                            }
                        }
                        buf[key_cursor_x] = '\0';
                        box_fill8(window->buffer, window->width, 40, 28, window->width, 28 + 16, COL8_C6C6C6);
                        box_fill8(window->buffer, window->width, 40 + key_cursor_x * 8, 28, 40 + (key_cursor_x + 1) * 8,
                                  28 + 16, COLOR_WHITE);
                        print_str(window->buffer, window->width, 40, 28, buf, COLOR_WHITE);
                        set_sheet_pos(window, window->x0, window->y0);
                    }
                    break;
                case FromTimer:
                    timer_ctl_tick(&GlobalTimerCallbackCtl);
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

// 制作窗口
void make_window8(char *buf, short xSize, short ySize, char *title) {
    static char closebtn[14][16] = {
            "OOOOOOOOOOOOOOO@",
            "OQQQQQQQQQQQQQ$@",
            "OQQQQQQQQQQQQQ$@",
            "OQQQ@@QQQQ@@QQ$@",
            "OQQQQ@@QQ@@QQQ$@",
            "OQQQQQ@@@@QQQQ$@",
            "OQQQQQQ@@QQQQQ$@",
            "OQQQQQ@@@@QQQQ$@",
            "OQQQQ@@QQ@@QQQ$@",
            "OQQQ@@QQQQ@@QQ$@",
            "OQQQQQQQQQQQQQ$@",
            "OQQQQQQQQQQQQQ$@",
            "O$$$$$$$$$$$$$$@",
            "@@@@@@@@@@@@@@@@"
    };
    int x, y;
    char c;
    box_fill8(buf, xSize, 0, 0, xSize - 1, 0, COL8_C6C6C6);
    box_fill8(buf, xSize, 1, 1, xSize - 2, 1, COL8_FFFFFF);
    box_fill8(buf, xSize, 0, 0, 0, ySize - 1, COL8_C6C6C6);
    box_fill8(buf, xSize, 1, 1, 1, ySize - 2, COL8_FFFFFF);
    box_fill8(buf, xSize, xSize - 2, 1, xSize - 2, ySize - 2, COL8_848484);
    box_fill8(buf, xSize, xSize - 1, 0, xSize - 1, ySize - 1, COL8_000000);
    box_fill8(buf, xSize, 2, 2, xSize - 3, ySize - 3, COL8_C6C6C6);
    box_fill8(buf, xSize, 3, 3, xSize - 4, 20, COL8_000084);
    box_fill8(buf, xSize, 1, ySize - 2, xSize - 2, ySize - 2, COL8_848484);
    box_fill8(buf, xSize, 0, ySize - 1, xSize - 1, ySize - 1, COL8_000000);
    print_str(buf, xSize, 24, 4, title, COL8_FFFFFF);
    for (y = 0; y < 14; y++) {
        for (x = 0; x < 16; x++) {
            c = closebtn[y][x];
            if (c == '@') {
                c = COL8_000000;
            } else if (c == '$') {
                c = COL8_848484;
            } else if (c == 'Q') {
                c = COL8_C6C6C6;
            } else {
                c = COL8_FFFFFF;
            }
            buf[(5 + y) * xSize + (xSize - 21 + x)] = c;
        }
    }
}
