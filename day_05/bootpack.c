// コンパイル時に -nostdlib を付与しているので，読み込めない．
// #include <stdio.h>

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

/* Call functions from external files. */
extern void io_hlt(void);
extern void io_cli(void);
extern void io_out8(int port, int data);
extern int io_load_eflags(void);
extern void io_store_eflags(int eflags);
extern void load_gdtr(int limit, int addr);
extern void load_idtr(int limit, int addr);
extern void re_sprintf(char *str, char *fmt, ...);

/* Call functions from internal files. */
void init_palette(void);
void init_screen(char *vram, int x, int y);
void init_mouse_cursor(char *mouse, char bc);
void init_gdtidt(void);

void set_palette(int start, int end, unsigned char *rgb);

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void putfonts8(char *vram, int xsize, int x, int y, char color, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char color, unsigned char *s);
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);

struct BOOTINFO {
    char cyls, leds, vmode, reserve;
    short scrnx, scrny;
    char *vram;
};

struct SEGMENT_DESCRIPTOR {
    short limit_low, base_low;
    char base_mid, access_right;
    char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
    short offset_low, selector;
    char dw_count, access_right;
    short offset_high;
};

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

void HariMain(void)
{
    struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
    char s[40], mcursor[256];
    int mx, my;

    init_gdtidt();
    init_palette();
    init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;

    init_mouse_cursor(mcursor, COL8_008484);
    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
    re_sprintf(s, "(%d, %d)", mx, my);
    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

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

void init_mouse_cursor(char *mouse, char bc)
{
    static char cursor[16][16] = {
        "**************..",   // 1
        "*ooooooooooo*...",   // 2
        "*oooooooooo*....",   // 3
        "*ooooooooo*.....",   // 4
        "*oooooooo*......",   // 5
        "*ooooooo*.......",   // 6
        "*ooooooo*.......",   // 7
        "*oooooooo*......",   // 8
        "*oooo**ooo*.....",   // 9
        "*ooo*..*ooo*....",   // 10
        "*oo*....*ooo*...",   // 11
        "*o*......*ooo*..",   // 12
        "**........*ooo*.",   // 13
        "*..........*ooo*",   // 14
        ".. .........*oo*",   // 15
        ".............***",   // 16
    };
    int x, y;

    for (y = 0; y < 16; y++) {
        for(x = 0; x < 16; x++) {
            if (cursor[y][x] == '*') {
                mouse[y * 16 + x] = COL8_000000;
            }
            if (cursor[y][x] == 'o') {
                mouse[y * 16 + x] = COL8_FFFFFF;
            }
            if (cursor[y][x] == '.') {
                mouse[y * 16 + x] = bc;
            }
        }
    }
    return;
}

void init_gdtidt(void)
{
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;
    struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR *) 0x0026f800;
    int i;

    // GDT 初期化
    for (i = 0; i < 8192; i++) {
        // `gdt` はポインタなので，i の値が 1 増えるごとに，8byte の構造体へのアクセスなので，メモリ番地が 8 増えているよ．
        set_segmdesc(gdt + i, 0, 0, 0);
    }
    set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
    set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
    load_gdtr(0xffff, 0x00270000);

    // IDT 初期化
    for (i = 0; i < 256; i++) {
        set_gatedesc(idt + i, 0, 0, 0);
    }
    load_idtr(0x7ff, 0x0026f800);
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

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
    if (limit > 0xfffff) {
        ar |= 0x8000; // G_bit = 1
        limit /= 0x1000;
    }
    sd->limit_low    = limit & 0xffff;
    sd->base_low     = base & 0xffff;
    sd->base_mid     = (base >> 16) & 0xff;
    sd->access_right = ar & 0xff;
    sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
    sd->base_high    = (base >> 24) & 0xff;
    return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
    gd->offset_low   = offset & 0xffff;
    gd->selector     = selector;
    gd->dw_count     = (ar >> 8) & 0xff;
    gd->access_right = ar & 0xff;
    gd->offset_high  = (offset >> 16) & 0xffff;
    return;
}

void putfonts8(char *vram, int xsize, int x, int y, char color, char *font)
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

void putfonts8_asc(char *vram, int xsize, int x, int y, char color, unsigned char *s)
{
    extern char hankaku[4096];
    for (; *s != 0x00; s++) {
        putfonts8(vram, xsize, x, y, color, hankaku + *s * 16);
        x += 8;
    }
    return;
}

void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize)
{
    int x,y;
    for (y = 0; y < pysize; y++) {
        for (x = 0; x < pxsize; x++) {
            vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
        }
    }
    return;
}
