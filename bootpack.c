//
// Created by liwei1 on 2021/3/27.
//
#include "bootpack.h"
#include "signalbuffer.h"
#include "naskfunc.h"
#include "graphic.h"
#include "dsctbl.h"
#include "int.h"
#include "kbc.h"
#include "mousedecoder.h"
#include "memorymanager.h"
#include "task.h"
#include "textbox.h"
#include "string.h"
#include "file.h"

unsigned int mem_test(unsigned int start, unsigned int end);

void init_mouse_sheet(struct SheetControl *sheet_control);

void make_window8(char *buf, short xSize, short ySize, char *title);

struct Sheet *create_window(short x0, short y0, short width, short height, char *title);

struct BootInfo *const Boot_Info_Ptr = (struct BootInfo *const) 0x00000ff0;

struct SheetControl *global_sheet_control;

const short MouseWidth = 16;
const short MouseHeight = 16;

#define MEMORY_MANAGER_ADDR            0x003c0000

void init_manager();

struct Sheet *mouse_sheet;

static char KeyTableWithoutShift[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0,
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', ',', 0, 0, '\\',
        'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0,
        '7', '8', '9', '-',
        '4', '5', '6', '+',
        '1', '2', '3',
        '0', '.',
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0x5c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x5c, 0, 0
};
static char KeyTableWithShift[] = {
        0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0,
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', 0, 0, '|',
        'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0,
        '7', '8', '9', '-',
        '4', '5', '6', '+',
        '1', '2', '3',
        '0', '.',
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0x5c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x5c, 0, 0
};


// 控制台
void console_task() {
    struct SignalBuffer *signal_buffer = order_signal();
    const short window_weight = 320;
    const short window_height = 240;
    const short title_bar_height = 32;   // 标题栏高度
    const short border = 16;   // 边框宽度
    bool exist = false;
    struct Sheet *console_window = create_window(100, 100, window_weight, window_height, "console");
    struct TextBox *textBox = newTextBox(console_window, border, title_bar_height, window_weight - border,
                                         window_height - border);
    *(struct TextBox **) (0x0fec) = textBox;
    // console_address = textBox;
    bool shift_key_down = false;    // 是否按下了 shift 键
    unsigned char input, type;
    while (1) {
        io_cli();       // 禁用中断
        exist = read_data_from_buffer(signal_buffer, &input, &type);
        if (exist) {
            io_sti();
            // 如果 buffer 内有数据
            if (type == FromKeyBoard) {
                if (input == 0x2a) {
                    // 按下 shift 键
                    shift_key_down = true;
                    continue;
                }
                if (input == 0xaa) {
                    // 松开 shift 键
                    shift_key_down = false;
                    continue;
                }
                if (input == 0x0e) {
                    // 退格键
                    handle_backspace(textBox);
                }
                if (input == 0x1c) {
                    // enter 键
                    if (strEqual(textBox->line_buffer, "dir")) {
                        handle_enter(textBox);
                        char s[100] = {0};
                        int x, y;
                        for (x = 0; x < 224; x++) {
                            if (FormatFileInfoToBuffer(BaseFileInfoAddress + x, s)) {
                                handle_new_line(textBox, s);
                                handle_redraw(textBox);
                            } else {
                                break;
                            }
                        }
                    } else if (strNEqual(textBox->line_buffer, "cat", 3)) {
                        handle_enter(textBox);
                        // 寻找空格的位置
                        int blankIndex = findIndex(textBox->line_buffer, ' ');
                        char *fileName = textBox->line_buffer + blankIndex + 1;
                        struct FileInfo *file = FindFileByName(fileName);
                        if (file == nullptr) {
                            handle_new_line(textBox, "file is not exist");
                        } else {
                            char *buffer = (char *) memory_alloc(global_memory_manager, 4096);
                            ReadFileIntoBuffer(file, buffer, 4096);
                            handle_new_line(textBox, buffer);
                            memory_free(global_memory_manager, (unsigned int) buffer, 4096);
                        }
                        handle_redraw(textBox);
                    } else if (strEqual(textBox->line_buffer, "hlt")) {
                        handle_enter(textBox);
                        handle_redraw(textBox);
                        struct FileInfo *file = FindFileByName("HLT.HRB");
                        char *buffer = (char *) memory_alloc(global_memory_manager, 4096);
                        ReadFileIntoBuffer(file, buffer, 4096);
                        set_code_desc(0, (int) buffer, file->size - 1);
                        call_code_segment(0);
                        memory_free(global_memory_manager, (unsigned int) buffer, 4096);
                    } else if (strEqual(textBox->line_buffer, "a")) {
                        handle_enter(textBox);
                        handle_redraw(textBox);
                        struct FileInfo *file = FindFileByName("A.HRB");
                        char *buffer = (char *) memory_alloc(global_memory_manager, 4096);
                        ReadFileIntoBuffer(file, buffer, 4096);
                        // 要替换前六个字节
                        buffer[0] = 0xe8;
                        buffer[1] = 0x16;
                        buffer[2] = 0x00;
                        buffer[3] = 0x00;
                        buffer[4] = 0x00;
                        buffer[5] = 0xcb;
                        set_code_desc(0, (int) buffer, file->size - 1);
                        call_code_segment(0);
                        memory_free(global_memory_manager, (unsigned int) buffer, 4096);
                    } else if (strEqual(textBox->line_buffer, "crack")) {
                        handle_enter(textBox);
                        handle_redraw(textBox);
                        struct FileInfo *file = FindFileByName("CRACK.HRB");
                        char *buffer = (char *) memory_alloc(global_memory_manager, 4096);
                        ReadFileIntoBuffer(file, buffer, 4096);
                        // 要替换前六个字节
                        buffer[0] = 0xe8;
                        buffer[1] = 0x16;
                        buffer[2] = 0x00;
                        buffer[3] = 0x00;
                        buffer[4] = 0x00;
                        buffer[5] = 0xcb;
                        set_code_desc(0, (int) buffer, file->size - 1);
                        call_code_segment(0);
                        memory_free(global_memory_manager, (unsigned int) buffer, 4096);
                    }else {
                        handle_enter(textBox);
                        handle_redraw(textBox);
                    }
                }
                // input < 0x54 是为了只接收按下的字符，不接受松开的字符
                if ((input < 0x54) && (KeyTableWithoutShift[input] != 0)) {
                    if (shift_key_down) {
                        handle_new_char_come(textBox, KeyTableWithShift[input]);
                    } else {
                        handle_new_char_come(textBox, KeyTableWithoutShift[input]);
                    }
                    handle_redraw(textBox);
                }
            }
        } else {
            // 如果 buffer 内没有数据 就继续睡觉
            io_stihlt();    // 原因见 readme
        }
    }
}

void init_sheet_control() {
    global_sheet_control = new_sheet_control(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX,
                                             Boot_Info_Ptr->screenY);
    // 背景图层
    struct Sheet *background_sheet = create_sheet(global_sheet_control, 0, 0, Boot_Info_Ptr->screenX,
                                                  Boot_Info_Ptr->screenY);
    set_sheet_color(background_sheet, COLOR_WHITE);
}

struct Sheet *create_window(short x0, short y0, short width, short height, char *title) {
    struct Sheet *window = create_sheet(global_sheet_control, x0, y0, width, height);
    set_sheet_color(window, COLOR_TRANSPARENT);
    make_window8(window->buffer, window->width, window->height, title);
    sheet_control_draw(global_sheet_control);
    return window;
}

void HariMain(void) {
    init_gdt();
    init_idt();
    init_pic();
    init_pit();
    init_palette();
    init_manager();
    InitGlobalTaskController();
    initSignalBufferController();
    AddTask((int) console_task, 2);
    init_sheet_control();
    initFatTable();
    init_mouse_sheet(global_sheet_control);
    init_keyboard();
    enable_mouse();

    run();
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

void run() {
    struct SignalBuffer *signal_buffer = order_signal();
    struct Sheet *window = create_window(30, 30, 160, 52, "main window");
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
        exist = read_data_from_buffer(signal_buffer, &input, &type);
        if (exist) {
            io_sti();
            // 如果 buffer 内有数据
            if (type == FromMouse) {
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
    // 表面上 "OOOOOOOOOOOOOOO@" 中只有 16 个 char，但是后面有一个 \0 。
    static char closebtn[14][17] = {
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
