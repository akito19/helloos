#include "bootpack.h"

#define PIT_CTRL  0x0043
#define PIT_CNT0  0x0040
#define TIMER_FLAGS_ALLOC  1  // timerを確保した状態
#define TIMER_FLAGS_USING  2  // timer作動中

struct TIMERCTL timerctl;

// 100Hz == 10ms ごとの割り込みを行う
void init_pit(void)
{
    int i;
    io_out8(PIT_CTRL, 0x34);
    io_out8(PIT_CNT0, 0x9c);
    io_out8(PIT_CNT0, 0x2e);
    timerctl.count = 0;
    timerctl.next = 0xffffff; // 最初は動作中のタイマがない
    timerctl.using = 0;
    for (i = 0; i < MAX_TIMER; i++) {
        timerctl.timers0[i].flags = 0; // 未使用
    }
    return;
}

struct TIMER *timer_alloc(void)
{
    int i;
    for (i = 0; i < MAX_TIMER; i++) {
        if (timerctl.timers0[i].flags == 0) {
            timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
            return &timerctl.timers0[i];
        }
    }
    return 0; // timerが見つからなかった
}

void timer_free(struct TIMER *timer)
{
    timer->flags = 0; // 未使用
    return;
}

void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data)
{
    timer->fifo = fifo;
    timer->data = data;
    return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout)
{
    int e;
    struct TIMER *t, *s;
    timer->timeout = timeout + timerctl.count;
    timer->flags = TIMER_FLAGS_USING;
    e = io_load_eflags();
    io_cli();
    timerctl.using++;
    if (timerctl.using == 1) {
        // 動作中のタイマはこれ1つになる場合
        timerctl.t0 = timer;
        timer->next = 0; // 次はない
        timerctl.next = timer->timeout;
        io_store_eflags(e);
        return;
    }
    t = timerctl.t0;
    if (timer->timeout <= t->timeout) {
        // 先頭に入れる場合（よりタイムアウトが短いものがセットされる場合？）
        timerctl.t0 = timer;
        timer->next = t;
        io_store_eflags(e);
        return;
    }
    // どこの timer に追加したらいいのか探す
    for (;;) {
        s = t;
        t = t->next;
        if (t == 0) {
            break; // 一番うしろになった
        }
        if (timer->timeout <= t->timeout) {
            // s, t の間に入れる場合
            s->next = timer;
            timer->next = t;
            io_store_eflags(e);
            return;
        }
    }
    // 一番うしろに入れる場合
    s->next = timer;
    timer->next = 0;
    io_store_eflags(e);
    return;
}

void inthandler20(int *esp)
{
    int i, j;
    struct TIMER *timer;
    io_out8(PIC0_OCW2, 0x60); // IRQ-00 の受付完了をPICに通知
    timerctl.count++;
    if (timerctl.next > timerctl.count) {
        return;
    }
    timer = timerctl.t0;  // 先頭番地をとりあえず timer に代入
    for (i = 0; i < timerctl.using; i++) {
        // timersのタイマはすべて動作中のものなので，flagsを確認しない
        if (timer->timeout > timerctl.count) {
            break;
        }
        // Timeout!
        timer->flags = TIMER_FLAGS_ALLOC;
        fifo32_put(timer->fifo, timer->data);
        timer = timer->next; // 次のタイマの番地をtimerに代入
    }
    // ちょうど i 個のタイマがタイムアウトしたので，残りをずらす
    timerctl.using -= i;
    timerctl.t0 = timer;

    // timerctl.next の設定
    if (timerctl.using > 0) {
        timerctl.next = timerctl.t0->timeout;
    } else {
        timerctl.next = 0xffffffff;
    }
    return;
}
