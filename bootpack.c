//
// Created by liwei1 on 2021/3/27.
//
void io_hlt(void);

void write_mem8(int addr, int data);

void HariMain(void) {
    char *p = (char *) 0xa0000;
    for (; p < (char *) 0xaffff; ++p) {
        *p = 15;
    }
    for (;;) {
        io_hlt();
    }
}