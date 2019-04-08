#include "bootpack.h"

struct FIFO8 keyfifo;

// Use following in this file only.
#define PORT_KEYSTA           0x0064
#define KEYSTA_SEND_NOTREADY  0x02
#define KEYCMD_WRITE_MODE     0x60
#define KBC_MODE              0x47

// PS/2キーボードからの割り込み
void inthandler21(int *esp)
{
    unsigned char data;
    io_out8(PIC0_OCW2, 0x61);  // IRQ-01受付完了PICに通知
    data = io_in8(PORT_KEYDAT);
    fifo8_put(&keyfifo, data); // `&` はstruct, variableのメモリ番地を教えてもらう
    return;
}

void wait_KBC_sendready(void)
{
    // keyboard controller がデータ送信可能になるのを待つ
    for(;;) {
        // 装置番号 0x0064 の下位2bitが0になるまで待つ
        if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
            break;
        }
    }
    return;
}

void init_keyboard(void)
{
    // Initialize keyboard controller
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);
    return;
}
