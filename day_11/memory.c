#include "bootpack.h"

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

unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size)
{
    unsigned int a;
    size = (size + 0xfff) & 0xfffff000;
    a = memman_alloc(man, size);
    return a;
}

int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
    int i;
    size = (size + 0xfff) & 0xfffff000;
    i = memman_free(man, addr, size);
    return i;
}
