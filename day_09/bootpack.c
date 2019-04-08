#include "bootpack.h"

#define MEMMAN_FREES  4092  // これで約32KB
#define MEMMAN_ADDR 0x003c0000

struct FREEINFO { // 空き情報
    unsigned int addr, size;
};
struct MEMMAN { // メモリ管理
    int frees, maxfrees, lostsize, losts;
    struct FREEINFO free[MEMMAN_FREES];
};

unsigned int memtest(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);

void HariMain(void)
{
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
    struct MOUSE_DEC mdec;
    struct MEMMAN *memman = (struct MEMMAM *) MEMMAN_ADDR;
    char s[40], mcursor[256], keybuf[32], mousebuf[128];
    int i, mx, my;
    unsigned int memtotal;

    init_gdtidt();
    init_pic();
    io_sti(); // Set Interrupt Flag, 有効時，外部装置からの割り込みを受け付けるようになる．
    fifo8_init(&keyfifo, 32, keybuf);
    fifo8_init(&mousefifo, 128, mousebuf);
    io_out8(PIC0_IMR, 0xf9);
    io_out8(PIC1_IMR, 0xef);

    init_keyboard();
    enable_mouse(&mdec);
    memtotal = memtest(0x00400000, 0xbfffffff);
    memman_init(memman);
    memman_free(memman, 0x00001000, 0x0009e000); // Make memory free bwtween 0x00001000 and 0x0009e000
    memman_free(memman, 0x00400000, memtotal - 0x00400000);

    init_palette();
    init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    init_mouse_cursor(mcursor, COL8_008484);
    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
    mysprintf(s, "(%d, %d)", mx, my);
    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

    i = memtest(0x00400000, 0xbfffffff) / (1024 * 1024);
    mysprintf(s, "memory %d MB   free: %d KB", memtotal / (1024 * 1024), memman_total(memman) / 1024);
    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 32, COL8_FFFFFF, s);

    for(;;) {
        io_cli();
        if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0) {
            io_stihlt();
        } else {
            if (fifo8_status(&keyfifo) != 0) {
                i = fifo8_get(&keyfifo);
                io_sti();
                mysprintf(s, "%x", i);
                boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
                putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16,  COL8_FFFFFF, s);
            } else if (fifo8_status(&mousefifo) != 0) {
                i = fifo8_get(&mousefifo);
                io_sti();
                if (mouse_decode(&mdec, i) != 0) {
                    // データが3バイト揃ったので表示
                    mysprintf(s, "[lcr %d %d]", mdec.x, mdec.y);
                    if ((mdec.btn & 0x01) != 0) {
                        s[1] = 'L';
                    }
                    if ((mdec.btn & 0x02) != 0) {
                        s[3] = 'R';
                    }
                    if ((mdec.btn & 0x04) != 0) {
                        s[2] = 'C';
                    }
                    boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);
                    putfonts8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
                    // マウスカーソルの移動
                    boxfill8(binfo->vram, binfo->scrnx, COL8_008484, mx, my, mx+15, my+15); // マウス消す
                    mx += mdec.x;
                    my += mdec.y;
                    if (mx < 0) {
                        mx = 0;
                    }
                    if (my < 0) {
                        my = 0;
                    }
                    if (mx > binfo->scrnx - 16) {
                        mx = binfo->scrnx - 16;
                    }
                    if (my > binfo->scrny - 16) {
                        my = binfo->scrny - 16;
                    }
                    mysprintf(s, "(%d, %d)", mx, my);
                    boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 0, 79, 15); // 座標消す
                    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s); // 座標書く
                    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16); // マウス描く
                }
            }
        }
    }
}

#define EFLAGS_AC_BIT      0x00040000
#define CR0_CACHE_DISABLE  0x60000000

unsigned int memtest(unsigned int start, unsigned int end)
{
    char flg486 = 0;
    unsigned int eflg, cr0, i;

    // プロセッサが 386 か 486 以降なのかの確認
    eflg = io_load_eflags();
    eflg |= EFLAGS_AC_BIT;  // AC_bit = 1
    io_store_eflags(eflg);
    eflg = io_load_eflags();
    if ((eflg & EFLAGS_AC_BIT) != 0) {
        // 386 では AC = 1 にしても自動で0に戻ってしまう
        flg486 = 1;
    }
    // `~` はbit反転
    // 0x00040000 -> 0xfffbffff
    eflg &= ~EFLAGS_AC_BIT; // AC-bot = 0
    io_store_eflags(eflg);

    if (flg486 != 0) {
        cr0 = load_cr0();
        cr0 |= CR0_CACHE_DISABLE; // cache無効化
        store_cr0(cr0);
    }

    i = memtest_sub(start, end);

    if (flg486 != 0) {
        cr0 = load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE; // cache 有効化
        store_cr0(cr0);
    }

    return i;
}

void memman_init(struct MEMMAN *man)
{
    man->frees = 0;    // 空き情報の個数
    man->maxfrees = 0; // 状況観察用: frees の最大値
    man->lostsize = 0; // 開放に失敗した合計サイズ
    man->losts = 0;    // 開放に失敗した回数
    return;
}

// 空きサイズの合計を報告
unsigned int memman_total(struct MEMMAN *man)
{
    unsigned int i, t = 0;
    for (i = 0; i < man->frees; i++) {
        t += man->free[i].size;
    }
    return t;
}

// メモリ確保
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
{
    unsigned int i, a;
    for (i = 0; i < man->frees; i++) {
        // 十分な広さの空きを発見
        if (man->free[i].size >= size) {
            a = man->free[i].addr;
            man->free[i].addr += size;
            man->free[i].size -= size;
            if (man->free[i].size == 0) {
                // free[i] がなくなったので前へ詰める
                man->frees--;
                for (; i < man->frees; i++) {
                    man->free[i] = man->free[i + 1]; // 構造体の代入
                }
            }
            return a;
        }
    }
    return 0; // 空きがない場合
}

// メモリ解放
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
    int i, j;
    // まとめやすさを考えると， `free[]` が `addr` 順に並んでいる方がいいので，
    // 先にどこのアドレスに入れるべきかを決める．
    for (i = 0; i < man->frees; i++) {
        if (man->free[i].addr > addr) {
            break;
        }
    }

    // free[i - 1].addr < addr < free[i].addr
    if (i > 0) {
        // 前のアドレスに空きがある
        if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
            // 前の空き領域にまとめられる
            man->free[i - 1].size += size;
            if (i < man->frees) {
                // 後ろにも空きがある
                if (addr + size == man->free[i].addr) {
                    // まとめられる
                    man->free[i - 1].size += man->free[i].size;
                    // man->free[i] を削除し，ここで free[i] がなくなるので前へつめる
                    man->frees--;
                    for (; i < man->frees; i++) {
                        man->free[i] = man->free[i + 1]; // 構造体の代入
                    }
                }
            }
            return 0; // 成功終了
        }
    }

    // 前のアドレスとはまとめられなかった
    if (i < man->frees) {
        // 後ろがある
        if (addr + size == man->free[i].addr) {
            // 後ろとはまとめられる
            man->free[i].addr = addr;
            man->free[i].size += size;
            return 0; // 成功終了
        }
    }

    // 前後のアドレスとまとめられない
    if (man->frees < MEMMAN_FREES) {
        // free[i] よりも後ろを，後ろへずらして隙間を作る
        for (j = man->frees; j > i; j++) {
            man->free[j] = man->free[j - 1];
        }
        man->frees++;
        if (man->maxfrees < man->frees) {
            man->maxfrees = man->frees; // 最大値を更新
        }
        man->free[i].addr = addr;
        man->free[i].size = size;
        return 0; // 成功終了
    }

    // 後ろにずらせなかった
    man->losts++;
    man->lostsize += size;
    return -1; // 失敗終了
}
