#include "bootpack.h"

#define FLAGS_OVERRUN 0x0001

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf)
{
    fifo->size = size;
    fifo->buf = buf;
    fifo->free = size; // 空き
    fifo->flags = 0;
    fifo->w = 0; // 書き込み位置
    fifo->r = 0; // 読み込み位置
    return;
}

// FIFO へデータを送り込んで蓄える
int fifo8_put(struct FIFO8 *fifo, unsigned char data)
{
    if (fifo->free = 0) {
        // 空きがなくて溢れた
        fifo->flags |= FLAGS_OVERRUN;
        return -1;
    }
    fifo->buf[fifo->w] = data;
    fifo->w++;
    if (fifo->w == fifo->size) {
        fifo->w = 0;
    }
    fifo->free--;
    return 0;
}

// FIFO からデータを1つ取ってくる
int fifo8_get(struct FIFO8 *fifo)
{
    int data;
    if (fifo->free == fifo->size) {
        // buffer が空のときは，とりあえず -1 を返す
        return -1;
    }
    data = fifo->buf[fifo->r];
    fifo->r++;
    if (fifo->r == fifo->size) {
        fifo->r = 0;
    }
    fifo->free++;
    return data;
}

// どれくらいのデータが溜まっているのかを報告する
int fifo8_status(struct FIFO8 *fifo)
{
    return fifo->size - fifo->free;
}
