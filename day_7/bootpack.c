// コンパイル時に -nostdlib を付与しているので，読み込めない．
// #include <stdio.h>
#include "bootpack.h"

struct FIFO8 keyfifo;

void HariMain(void)
{
    struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
    char s[40], mcursor[256], keybuf[32];
    int i, mx, my;

    init_gdtidt();
    init_pic();
    io_sti(); // Set Interrupt Flag, 有効時，外部装置からの割り込みを受け付けるようになる．

    init_palette();
    init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
    fifo8_init(&keyfifo, 32, keybuf);

    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;

    init_mouse_cursor(mcursor, COL8_008484);
    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
    re_sprintf(s, "(%d, %d)", mx, my);
    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

    io_out8(PIC0_IMR, 0xf9);
    io_out8(PIC1_IMR, 0xef);

    for(;;) {
        io_cli();
        if (fifo8_status(&keyfifo) == 0) {
            io_stihlt();
        } else {
            i = fifo8_get(&keyfifo);
            io_sti();
            re_sprintf(s, "%x", i);
            boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
            putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16,  COL8_FFFFFF, s);
        }
    }
}
