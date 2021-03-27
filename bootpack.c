//
// Created by liwei1 on 2021/3/27.
//
void io_hlt(void);

void write_mem8(int addr, int data);

void HariMain(void) {
    int i;
    // 估计是编译器落后了  for (int i = 0xa0000; i <= 0xaffff; ++i) 死活编译不过
    for (i = 0xa0000; i <= 0xaffff; ++i) {
        write_mem8(i, 15); /* MOV BYTE [i],15 */
    }
    for (;;) {
        io_hlt();
    }
}