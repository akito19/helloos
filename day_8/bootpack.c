#include "bootpack.h"

struct FIFO8 keyfifo;
struct FIFO8 mousefifo;
void init_keyboard(void);
void enable_mouse(void);

void HariMain(void)
{
    struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
    char s[40], mcursor[256], keybuf[32], mousebuf[128];
    unsigned char mouse_dbuf[3], mouse_phase;
    int i, mx, my;

    init_gdtidt();
    init_pic();
    io_sti(); // Set Interrupt Flag, 有効時，外部装置からの割り込みを受け付けるようになる．
    fifo8_init(&keyfifo, 32, keybuf);
    fifo8_init(&mousefifo, 128, mousebuf);
    io_out8(PIC0_IMR, 0xf9);
    io_out8(PIC1_IMR, 0xef);

    init_keyboard();

    init_palette();
    init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    init_mouse_cursor(mcursor, COL8_008484);
    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
    re_sprintf(s, "(%d, %d)", mx, my);
    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

    enable_mouse();
    mouse_phase = 0; // マウスの 0xfa を待っている段階

    for(;;) {
        io_cli();
        if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0) {
            io_stihlt();
        } else {
            if (fifo8_status(&keyfifo) != 0) {
                i = fifo8_get(&keyfifo);
                io_sti();
                re_sprintf(s, "%x", i);
                boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
                putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16,  COL8_FFFFFF, s);
            } else if (fifo8_status(&mousefifo) != 0) {
                i = fifo8_get(&mousefifo);
                io_sti();
                if (mouse_phase == 0) {
                    // マウスの 0xfa を待っている段階
                    if (i == 0xfa) {
                        mouse_phase = 1;
                    }
                } else if (mouse_phase == 1) {
                    // マウスの1バイト目を待っている段階
                    mouse_dbuf[0] = i;
                    mouse_phase = 2;
                } else if (mouse_phase == 2) {
                    // 2バイト目ry
                    mouse_dbuf[1] = i;
                    mouse_phase = 3;
                } else if (mouse_phase == 3) {
                    mouse_dbuf[2] = i;
                    mouse_phase = 1;
                    // データが3バイト揃ったので表示
                    re_sprintf(s, "%x %x %x", mouse_dbuf[0], mouse_dbuf[1], mouse_dbuf[2]);
                    boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 32 + 8 * 8 - 1, 31);
                    putfonts8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
                }
            }
        }
    }
}

#define PORT_KEYDAT           0x0060
#define PORT_KEYSTA           0x0064
#define PORT_KEYCMD           0x0064
#define KEYSTA_SEND_NOTREADY  0x02
#define KEYCMD_WRITE_MODE     0x60
#define KBC_MODE              0x47

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

#define KEYCMD_SENDTO_MOUSE  0xd4
#define MOUSECMD_ENABLE      0xf4

void enable_mouse(void)
{
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
    return; // うまくいくとACK(0xfa)が送信されてくる
}
