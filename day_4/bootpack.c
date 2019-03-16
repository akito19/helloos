/* Call functions from external files. */
extern void io_hlt(void);
extern void io_cli(void);
extern void io_out8(int port, int data);
extern int io_load_eflags(void);
extern void io_store_eflags(int eflags);

/* Call functions from internal files. */
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);

#define COL8_000000     0
#define COL8_FF0000     1
#define COL8_00FF00     2
#define COL8_FFFF00     3
#define COL8_0000FF     4
#define COL8_FF00FF     5
#define COL8_00FFFF     6
#define COL8_FFFFFF     7
#define COL8_C6C6C6     8
#define COL8_840000     9
#define COL8_008400     10
#define COL8_848400     11
#define COL8_000084     12
#define COL8_840084     13
#define COL8_008484     14
#define COL8_848484     15

void HariMain(void)
{
    char *vram; /* `BYTE [...]` 用メモリ番地 */
    int xsize, ysize;

    init_palette();
    vram = (char *) 0xa0000;  /* メモリ番地を代入 */
    xsize = 320;
    ysize = 200;

    boxfill8(vram, xsize, COL8_008484, 0,          0, xsize - 1, ysize - 29);
    boxfill8(vram, xsize, COL8_C6C6C6, 0, ysize - 28, xsize - 1, ysize - 28);
    boxfill8(vram, xsize, COL8_FFFFFF, 0, ysize - 27, xsize - 1, ysize - 27);
    boxfill8(vram, xsize, COL8_C6C6C6, 0, ysize - 26, xsize - 1, ysize - 1);

    boxfill8(vram, xsize, COL8_FFFFFF,  3, ysize - 24, 59, ysize - 24);
    boxfill8(vram, xsize, COL8_FFFFFF,  2, ysize - 24,  2, ysize -  4);
    boxfill8(vram, xsize, COL8_848484,  3, ysize -  4, 59, ysize -  4);
    boxfill8(vram, xsize, COL8_848484, 59, ysize - 23, 59, ysize -  5);
    boxfill8(vram, xsize, COL8_000000,  2, ysize -  3, 59, ysize -  3);
    boxfill8(vram, xsize, COL8_000000, 69, ysize - 24, 60, ysize -  3);

    boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 24, xsize -  4, ysize - 24);
    boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize -  4);
    boxfill8(vram, xsize, COL8_FFFFFF, xsize - 47, ysize -  3, xsize -  4, ysize -  3);
    boxfill8(vram, xsize, COL8_FFFFFF, xsize -  3, ysize - 24, xsize -  3, ysize -  3);

    for(;;) {
        io_hlt;
    }
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
    int x, y;
    for (y = y0; y <= y1; y++) {
        for (x = x0; x<= x1; x++) {
            vram[y * xsize + x] = c;
        }
    }
    return;
}

void init_palette(void)
{
    /* 16進の2数 x 3 == 24bit color code つまり #ffffff みたいなやつ */
    static unsigned char table_rgb[16 * 3] = {
        0x00, 0x00, 0x00, /*  0: 黒, #000000 */
        0xff, 0x00, 0x00, /*  1: 明るい赤, #ff0000 */
        0x00, 0xff, 0x00, /*  2: 明るい緑, #00ff00 */
        0xff, 0xff, 0x00, /*  3: 明るい黄色, #ffff00 */
        0x00, 0x00, 0xff, /*  4: 明るい青, #0000ff */
        0xff, 0x00, 0xff, /*  5: 明るい紫, #ff00ff */
        0x00, 0xff, 0xff, /*  6: 明るい水色, #00ffff */
        0xff, 0xff, 0xff, /*  7: 白, #ffffff */
        0xc6, 0xc6, 0xc6, /*  8: 明るい灰色, #c6c6c6 */
        0x84, 0x00, 0x00, /*  9: 暗い赤, #840000 */
        0x00, 0x84, 0x00, /* 10: 暗い緑, #008400 */
        0x84, 0x84, 0x00, /* 11: 暗い黄色, #848400 */
        0x00, 0x00, 0x84, /* 12: 暗い青, #000084 */
        0x84, 0x00, 0x84, /* 13: 暗い紫, #840084 */
        0x00, 0x84, 0x84, /* 14: 暗い水色, #008484 */
        0x84, 0x84, 0x84, /* 15: 暗い灰色, #848484 */
    };

    /* static char 命令はデータにしか使えないが `DB` 命令相当 */
    set_palette(0, 15, table_rgb);
    return;
}

void set_palette(int start, int end, unsigned char *rgb)
{
    int i, eflags;
    eflags = io_load_eflags(); /* 割り込み許可フラグの値を記録する */
    io_cli();                  /* 許可フラグを0にして，割り込み禁止にする．CLI: Clear interrupt flag */
    io_out8(0x03c8, start);
    for (i = start; i <= end; i++) {
        io_out8(0x03c9, rgb[0] / 4);
        io_out8(0x03c9, rgb[1] / 4);
        io_out8(0x03c9, rgb[2] / 4);
        rgb += 3;
    }
    io_store_eflags(eflags); /* 割り込み許可フラグを元に戻す */
    return;
}
