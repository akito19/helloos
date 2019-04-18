#include "bootpack.h"

#define FLAGS_OVERRUN 0x0001

void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task)
{
    fifo->size = size;
    fifo->buf = buf;
    fifo->free = size; // 空き
    fifo->flags = 0;
    fifo->w = 0; // 書き込み位置
    fifo->r = 0; // 読み込み位置
    fifo->task = task; // データが入ったときに起こすタスク
    return;
}

// FIFO へデータを送り込んで蓄える
int fifo32_put(struct FIFO32 *fifo, int data)
{
    if (fifo->free == 0) {
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
    if (fifo->task != 0) {
        if (fifo->task->flags != 2) { // task がスリープしている場合
            task_run(fifo->task);
        }
    }
    return 0;
}

// FIFO からデータを1つ取ってくる
int fifo32_get(struct FIFO32 *fifo)
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
int fifo32_status(struct FIFO32 *fifo)
{
    return fifo->size - fifo->free;
}
