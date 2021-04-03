//
// Created by liwei1 on 2021/4/3.
//

#include "kbc.h"
#include "global.h"
#include "naskfunc.h"

const int KEY_STATUS_PORT = 0x0064;
const int KEY_CMD_PORT = 0x0064;
const int KEY_DAT_PORT = 0x0060;
const int KBC_MODE = 0x47;  // 模式设定的指令是0x60，利用鼠标模式的模式号码是0x47
const int KEY_CMD_SENDTO_MOUSE = 0xd4;
const int KEY_CMD_WRITE_MODE = 0x60;
const int MOUSE_CMD_ENABLE = 0xf4;
const unsigned char KEY_STATUS_SEND_NOT_READY = 0x02;

void wait_kbc_send_ready() {
    while (true) {
        unsigned char status = io_in8(KEY_STATUS_PORT);
        if ((status & KEY_STATUS_SEND_NOT_READY) == 0) {
            break;
        }
    }
}

void init_keyboard() {
    wait_kbc_send_ready();
    io_out8(KEY_CMD_PORT, KEY_CMD_WRITE_MODE);
    wait_kbc_send_ready();
    io_out8(KEY_DAT_PORT, KBC_MODE);
}

void enable_mouse() {
    wait_kbc_send_ready();
    io_out8(KEY_CMD_PORT, KEY_CMD_SENDTO_MOUSE);
    wait_kbc_send_ready();
    io_out8(KEY_DAT_PORT, MOUSE_CMD_ENABLE);
}
