/* Call functions from external files. */
extern void io_hlt(void);
extern void io_cli(void);
extern void io_out8(int port, int data);
extern int io_load_eflags(void);
extern void io_store_eflags(int eflags);

/* Call functions from internal files. */
void init_palette(void);
void init_screen(char *vram, int x, int y);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void put_font8(char *vram, int xsize, int x, int y, char color, char *font);

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

struct BOOTINFO {
    char cyls, leds, vmode, reserve;
    short scrnx, scrny;
    char *vram;
};

void HariMain(void)
{
    struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
    extern char hankaku[4096];

    init_palette();
    init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
    put_font8(binfo->vram, binfo->scrnx,  8, 8, COL8_FFFFFF, hankaku + 'A' * 16);
    put_font8(binfo->vram, binfo->scrnx, 16, 8, COL8_FFFFFF, hankaku + 'B' * 16);
    put_font8(binfo->vram, binfo->scrnx, 24, 8, COL8_FFFFFF, hankaku + 'C' * 16);
    put_font8(binfo->vram, binfo->scrnx, 40, 8, COL8_FFFFFF, hankaku + '1' * 16);
    put_font8(binfo->vram, binfo->scrnx, 48, 8, COL8_FFFFFF, hankaku + '2' * 16);
    put_font8(binfo->vram, binfo->scrnx, 56, 8, COL8_FFFFFF, hankaku + '3' * 16);

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

void init_screen(char *vram, int x, int y)
{
    boxfill8(vram, x, COL8_008484,      0,      0, x -  1, y - 29);
    boxfill8(vram, x, COL8_C6C6C6,      0, y - 28, x -  1, y - 28);
    boxfill8(vram, x, COL8_FFFFFF,      0, y - 27, x -  1, y - 27);
    boxfill8(vram, x, COL8_C6C6C6,      0, y - 26, x -  1, y -  1);

    boxfill8(vram, x, COL8_FFFFFF,      3, y - 24,     59, y - 24);
    boxfill8(vram, x, COL8_FFFFFF,      2, y - 24,      2, y -  4);
    boxfill8(vram, x, COL8_848484,      3, y -  4,     59, y -  4);
    boxfill8(vram, x, COL8_848484,     59, y - 23,     59, y -  5);
    boxfill8(vram, x, COL8_000000,      2, y -  3,     59, y -  3);
    boxfill8(vram, x, COL8_000000,     69, y - 24,     60, y -  3);

    boxfill8(vram, x, COL8_848484, x - 47, y - 24, x -  4, y - 24);
    boxfill8(vram, x, COL8_848484, x - 47, y - 23, x - 47, y -  4);
    boxfill8(vram, x, COL8_FFFFFF, x - 47, y -  3, x -  4, y -  3);
    boxfill8(vram, x, COL8_FFFFFF, x -  3, y - 24, x -  3, y -  3);

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

void put_font8(char *vram, int xsize, int x, int y, char color, char *font)
{
    int i;
    char *p, d; // data
    for (i = 0; i < 16; i++) {
        p = vram + (y + i) * xsize + x;
        d = font[i];
        if ((d & 0x80) != 0) { p[0] = color; }
        if ((d & 0x40) != 0) { p[1] = color; }
        if ((d & 0x20) != 0) { p[2] = color; }
        if ((d & 0x10) != 0) { p[3] = color; }
        if ((d & 0x08) != 0) { p[4] = color; }
        if ((d & 0x04) != 0) { p[5] = color; }
        if ((d & 0x02) != 0) { p[6] = color; }
        if ((d & 0x01) != 0) { p[7] = color; }
    }
    return;
}
