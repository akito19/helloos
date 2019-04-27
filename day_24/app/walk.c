int api_openwin(char *buf, int xsize, int ysize, int col_inv, char *title);
void api_initmalloc(void);
char *api_malloc(int size);
void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);
void api_closewin(int win);
void api_putstrwin(int win, int x, int y, int col, int len, char *str);
int api_getkey(int mode);
void api_end(void);

void HariMain(void)
{
    char *buf;
    int win, i, x, y;
    api_initmalloc();
    buf = api_malloc(160 * 100);
    win = api_openwin(buf, 160, 100, -1, "walk");
    api_boxfilwin(win, 4, 24, 155, 95, 0);
    x = 76;
    y = 56;
    api_putstrwin(win, x, y, 3, 1, "*");
    for (;;) {
        i = api_getkey(1);
        api_putstrwin(win, x, y, 0, 1, "*");
        if (i == 'h' && x > 4) { x -= 8; }
        if (i == 'l' && x < 148) { x += 8; }
        if (i == 'k' && y > 24) { y -= 8; }
        if (i == 'j' && x < 80) { y += 8; }
        if (i == 0x0a) { break; }
        api_putstrwin(win, x, y, 3, 1, "*");
    }
    api_closewin(win);
    api_end();
}
