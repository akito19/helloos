#include "bootpack.h"

// Initialize pic
void init_pic(void)
{
    io_out8(PIC0_IMR,  0xff);   // すべての割り込みを受け付けない
    io_out8(PIC1_IMR,  0xff);   // すべての割り込みを受け付けない

    io_out8(PIC0_ICW1, 0x11);   // エッジトリガモード
    io_out8(PIC0_ICW2, 0x20);   // IRQ0-7 は INT20-27 で受ける
    io_out8(PIC0_ICW3, 1 << 2); // PIC1 は IRQ2 にて接続
    io_out8(PIC0_ICW4, 0x01);   // ノンバッファモード

    io_out8(PIC1_ICW1, 0x11);   // エッジトリガモード
    io_out8(PIC1_ICW2, 0x28);   // IRQ8-15 は INT28-2f で受ける
    io_out8(PIC1_ICW3, 2);      // PIC1 は IRQ2 にて接続
    io_out8(PIC1_ICW4, 0x01);   // ノンバッファモード

    io_out8(PIC0_IMR,  0xfb);   // 11111011 PIC1 以外はすべて禁止
    io_out8(PIC1_IMR,  0xff);   // 11111111 すべての割り込みを受け付けない

    return;
}

void inthandler27(int *esp)
{
    io_out8(PIC0_OCW2, 0x67);
    return;
}
