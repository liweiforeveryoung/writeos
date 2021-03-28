//
// Created by liwei1 on 2021/3/27.
//

const char COL8_000000 = 0;    // 黑
const char COL8_FF0000 = 1;    // 亮红
const char COL8_00FF00 = 2;    // 亮绿
const char COL8_FFFF00 = 3;    // 亮黄
const char COL8_0000FF = 4;    // 亮蓝
const char COL8_FF00FF = 5;    // 亮紫
const char COL8_00FFFF = 6;    // 浅亮蓝
const char COL8_FFFFFF = 7;    // 白色
const char COL8_C6C6C6 = 8;    // 亮灰
const char COL8_840000 = 9;    // 暗红
const char COL8_008400 = 10;    // 暗绿
const char COL8_848400 = 11;    // 暗黄
const char COL8_000084 = 12;    // 暗青
const char COL8_840084 = 13;    // 暗紫
const char COL8_008484 = 14;    // 浅暗蓝
const char COL8_848484 = 15;    // 暗灰

void io_hlt(void);

void io_cli(void);

void io_out8(int port, int data);

int io_load_eflags(void);

void io_store_eflags(int eflags);

void box_fill8(unsigned char *vRam, int xSize, unsigned char c, int x0, int y0, int x1, int y1);

void init_palette(void);

void set_white_background(void);

void PrintChar(unsigned char *vRam, short xSize, short x0, short y0, const char *font, char color);

// 跑起来 yo yo yo checkout
void run();

unsigned char *const VRam_Addr_Begin = (unsigned char *const) 0xa0000;
unsigned char *const VRam_Addr_End = (unsigned char *const) 0xaffff;

struct BootInfo {
    char cyls;          // 磁盘的柱面数， ipl10.nas 中将柱面数写入了该地址
    char ledS;          // 指示灯的状态
    char vMode;         // 关于颜色数目的信息，颜色的位数
    char reverse;       // 保留的字段
    short screenX;      // 分辨率的 x (screen x)
    short screenY;      // 分辨率的 y (screen y)
    unsigned char *vRamAddr;     // 图像缓冲区的开始地址
};

struct BootInfo *const Boot_Info_Ptr = (struct BootInfo *const) 0x0ff0;

const char Font_A[16] = {
        0x00, 0x18, 0x18, 0x18, 0x18, 0x24, 0x24, 0x24,
        0x24, 0x7e, 0x42, 0x42, 0x42, 0xe7, 0x00, 0x00
};

void HariMain(void) {
    init_palette();
    set_white_background();
    box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, COL8_FF0000, 20, 20, 120, 120);
    box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, COL8_00FF00, 70, 50, 170, 150);
    box_fill8(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, COL8_0000FF, 120, 80, 220, 180);
    PrintChar(Boot_Info_Ptr->vRamAddr, Boot_Info_Ptr->screenX, 10, 10, Font_A, COL8_000000);
    run();
}

// 将背景设置为白色
void set_white_background(void) {
    unsigned char *p = VRam_Addr_Begin;
    for (; p < VRam_Addr_End; ++p) {
        *p = COL8_FFFFFF;
    }
}

// 设置调色板
void init_palette(void) {
    const unsigned char palette_table[16][3] = {
            {0x00, 0x00, 0x00},   // 黑
            {0xff, 0x00, 0x00},   // 亮红
            {0x00, 0xff, 0x00},   // 亮绿
            {0xff, 0xff, 0x00},   // 亮黄
            {0x00, 0x00, 0xff},   // 亮蓝
            {0xff, 0x00, 0xff},   // 亮紫
            {0x00, 0xff, 0xff},   // 浅亮蓝
            {0xff, 0xff, 0xff},   // 白色
            {0xc6, 0xc6, 0xc6},   // 亮灰
            {0x84, 0x00, 0x00},   // 暗红
            {0x00, 0x84, 0x00},   // 暗绿
            {0x84, 0x84, 0x00},   // 暗黄
            {0x00, 0x00, 0x84},   // 暗青
            {0x84, 0x00, 0x84},   // 暗紫
            {0x00, 0x84, 0x84},   // 浅暗蓝
            {0x84, 0x84, 0x84}    // 暗灰
    };
    int i = 0;
    int eFlags = io_load_eflags();
    io_cli();
    io_out8(0x03c8, 0); // 从 0 号调色板开始设置
    for (; i < 16; ++i) {
        io_out8(0x03c9, palette_table[i][0]);   // todo 教材中把这些值都除了 4 不懂为什么要除 4？
        io_out8(0x03c9, palette_table[i][1]);
        io_out8(0x03c9, palette_table[i][2]);
    }
    io_store_eflags(eFlags);
}

void box_fill8(unsigned char *vRam, int xSize, unsigned char c, int x0, int y0, int x1, int y1) {
    int x, y;
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++)
            vRam[y * xSize + x] = c;
    }
}

// 打印字体
void PrintChar(unsigned char *vRam, short xSize, short x0, short y0, const char *font, char color) {
    const short font_width = 8;
    const short font_height = 16;
    short x, y;
    for (y = y0; y < y0 + font_height; ++y) {
        unsigned char flag = 0x80;
        for (x = x0; x < x0 + font_width; ++x) {
            unsigned char *fontAddr = vRam + y * xSize + x;
            if ((font[y - y0] & flag) != 0) {
                *fontAddr = color;
            }
            flag = flag >> 1;
        }
    }
}

void run() {
    while (1) {
        io_hlt();
    }
}