//
// Created by liwei1 on 2021/3/27.
//

const int COL8_000000 = 0;    // 黑
const int COL8_FF0000 = 1;    // 亮红
const int COL8_00FF00 = 2;    // 亮绿
const int COL8_FFFF00 = 3;    // 亮黄
const int COL8_0000FF = 4;    // 亮蓝
const int COL8_FF00FF = 5;    // 亮紫
const int COL8_00FFFF = 6;    // 浅亮蓝
const int COL8_FFFFFF = 7;    // 白色
const int COL8_C6C6C6 = 8;    // 亮灰
const int COL8_840000 = 9;    // 暗红
const int COL8_008400 = 10;    // 暗绿
const int COL8_848400 = 11;    // 暗黄
const int COL8_000084 = 12;    // 暗青
const int COL8_840084 = 13;    // 暗紫
const int COL8_008484 = 14;    // 浅暗蓝
const int COL8_848484 = 15;    // 暗灰

void io_hlt(void);

void io_cli(void);

void io_out8(int port, int data);

int io_load_eflags(void);

void io_store_eflags(int eflags);

void box_fill8(unsigned char *vRam, int xSize, unsigned char c, int x0, int y0, int x1, int y1);

void init_palette(void);

void set_white_background(void);

// 跑起来 yo yo yo checkout
void run();

unsigned char *const VRam_Addr_Begin = (unsigned char *const) 0xa0000;
unsigned char *const VRam_Addr_End = (unsigned char *const) 0xaffff;

void HariMain(void) {
    init_palette();
    set_white_background();
    box_fill8(VRam_Addr_Begin, 320, COL8_FF0000, 20, 20, 120, 120);
    box_fill8(VRam_Addr_Begin, 320, COL8_00FF00, 70, 50, 170, 150);
    box_fill8(VRam_Addr_Begin, 320, COL8_0000FF, 120, 80, 220, 180);
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

void run() {
    while (1) {
        io_hlt();
    }
}