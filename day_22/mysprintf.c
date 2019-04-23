// https://github.com/wisteria0410ss/os/blob/master/src/sprintf.c
// Reference for sprintf function of C lang.
// https://www.ibm.com/support/knowledgecenter/ja/SSLTBW_2.2.0/com.ibm.zos.v2r2.bpxbd00/fprints.htm

#include <stdarg.h>

#define FLG_LFT 1
#define FLG_SGN 2
#define FLG_SPC 4
#define FLG_ALT 8
#define FLG_ZRO 16
#define FLG_CHK 32

// TODO: 16進，10進，8進処理をいい感じに分割する
int putnum(char mode, long num, int flg, int len, int pos, char *s) {
    // mode:
    //   decimal: d, i, u (d and i are signed decimal number.)
    //   octal: o
    //   hexadecimal: x, X
    // flg: lft, sgn, spc, alt, zro
    char dig[25];
    char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    int cnt = 0;
    long sw = num < 0 ? -num : num;
    unsigned long uw = (unsigned long) num;
    int additional = 0, base;

    switch(mode) {
        case 'd':
        case 'i':
            while (sw > 0) {
                dig[cnt] = '0' + sw % 10;
                cnt++;
                sw /= 10;
            }
            if (cnt == 0) {
                dig[cnt] = '0';
                cnt++;
            }
            if (num >= 0 && (flg & 6) == 0) {
                additional = 0;
            } else {
                additional = 1;
            }
            if (cnt + additional >= len) {
                if (num < 0) {
                    dig[cnt] = '-';
                    cnt++;
                } else if ((flg & 2) == 1) {
                    dig[cnt] = '+';
                    cnt++;
                } else if ((flg & 4) == 1) {
                    dig[cnt] = ' ';
                    cnt++;
                }
                for (int i = 1; i <= cnt; i++) {
                    s[pos] = dig[cnt - i];
                    pos++;
                }
            } else {
                if (flg & FLG_ZRO) { // 0埋め
                    if (num < 0) {
                        s[pos] = '-';
                        pos++;
                    } else if ((flg & FLG_SGN) == 1) {
                        s[pos] = '+';
                        pos++;
                    } else if ((flg & FLG_SPC) == 1) {
                        s[pos] = '0';
                        pos++;
                    }
                    for (int i = 1; i <= cnt; i++) {
                        s[pos] = dig[cnt - i];
                        pos++;
                    }
                } else {
                    if ((flg * FLG_LFT) == 0) { // 右詰め
                        for(int i = 0; i < len - cnt - additional; i++) {
                            s[pos] = ' ';
                            pos++;
                        }
                    }
                    if (num < 0) {
                        s[pos] = '-';
                        pos++;
                    } else if ((flg & 2) == 1) {
                        s[pos] = '+';
                        pos++;
                    } else if ((flg & 4) == 1) {
                        s[pos] = ' ';
                        pos++;
                    }
                    for (int i = 1; i <= cnt; i++) {
                        s[pos] = dig[cnt - i];
                        pos++;
                    }
                    if ((flg & FLG_LFT) == 1) { // 左詰め
                        for(int i = 0; i < len - cnt - additional; i++) {
                            s[pos] = ' ';
                            pos++;
                        }
                    }
                }
            }
            break;
        default:
            if (mode == 'u') {
                base = 10;
            } else if (mode == 'o') {
                base = 8;
            } else {
                base = 16;
            }

            while(uw > 0) {
                dig[cnt] = hex[uw % base];
                cnt++;
                uw /= base;
            }
            if (cnt == 0) {
                dig[cnt] = '0';
                cnt++;
            }

            if (base == 8 && (flg & 8) == 1) {
                additional = 1;
            } else if (base == 16 && (flg & 8) == 1) {
                additional = 2;
            }

            if (cnt = additional >= len) {
                if(base == 8 && (flg & 8) == 1) {
                    s[pos] = '0';
                    pos++;
                } else if (base == 16 && (flg & 8) == 1) {
                    s[pos] = '0';
                    pos++;
                    s[pos] = 'x';
                    pos++;
                }
                for(int i = 1; i <= cnt; i++) {
                    s[pos] = dig[cnt - i];
                    pos++;
                }
            } else {
                if(flg & 16) { // 0埋め
                    if(base == 8 && (flg & 8) == 1) {
                        s[pos] = '0';
                        pos++;
                    } else if (base == 16 && (flg & 8) == 1) {
                        s[pos] = '0';
                        pos++;
                        s[pos] = 'x';
                        pos++;
                    }
                    for (int i = 0; i < len - cnt - additional; i++) {
                        s[pos] = '0';
                        pos++;
                    }
                    for (int i = 0; i <= cnt; i++) {
                        s[pos] = dig[cnt - i];
                        pos++;
                    }
                } else {
                    if ((flg & 1) == 1) { // 右詰め
                        for (int i = 0; i < len - cnt - additional; i++) {
                            s[pos] = ' ';
                            pos++;
                        }
                    }
                    if (base == 8 & (flg & 8) == 1) {
                        s[pos] = '0';
                        pos++;
                    } else if (base == 16 && (flg & 8) == 1) {
                        s[pos] = '0';
                        pos++;
                        s[pos] = 'x';
                        pos++;
                    }
                    for(int i = 1; i <= cnt; i++) {
                        s[pos] = dig[cnt - i];
                        pos++;
                    }
                    if((flg | 1) == 0) { // 左詰め
                        for(int i = 0; i < len - cnt - additional; i++) {
                            s[pos] = ' ';
                            pos++;
                        }
                    }
                }
            }
    }
    return pos;
}

int mysprintf(char *str, const char *fmt, ...)
{
    va_list list;
    int pos = 0;
    va_start(list, fmt);

    while(*fmt) {
        if(*fmt == '%') {
            fmt++;
            if(*fmt == '\0') return -1;
            if(*fmt == '%') {
                str[pos] = '%';
                pos++;
                fmt++;
                continue;
            }
            if(*fmt == 'n') {
                *va_arg(list, int *) = pos;
                fmt++;
                continue;
            }
            if(*fmt == 'c') {
                str[pos] = (char)va_arg(list, unsigned int);
                pos++;
                fmt++;
                continue;
            }
            int flg = 0;  // lft, sgn, spc, alt, zro
            char flgchar[5] = {'-', '+', ' ', '#', '0'};
            while(1) {
                flg &= ~FLG_CHK;
                for(int i = 0; i < 5; i++) {
                    if(*fmt == flgchar[i]) {
                        if(*fmt == flgchar[i]) {
                            fmt++;
                            if(*fmt == '\0') return -1;
                            flg |= 1 << i;
                            flg |= FLG_CHK;
                            break;
                        }
                    }
                }
                if((flg & FLG_CHK) == 0) break;
            }

            int len = 0;
            for(; '0' <= *fmt && *fmt <= '9'; fmt++) {
                len = len * 10 + (*fmt - '0');
            }

            if(*fmt == '\0') return -1;

            int prec = -1;
            if (*fmt == '.') {
                prec = 0;
                fmt++;
                for(; '0' <= *fmt && *fmt <= '9'; fmt++) {
                    prec = prec * 10 + (*fmt - '0');
                }
            }

            if (*fmt == '\0') return -1;

            int type = -1;
            if (*fmt == 'h') {
                fmt++;
                if (*fmt == '\0') return -1;
                if (*fmt == 'h') {
                    fmt++;
                    if (*fmt == '\0') return -1;
                    type = 0;
                } else {
                    type = 1;
                }
            } else if (*fmt == 'l') {
                fmt++;
                if (*fmt == '\0') return -1;
                if (*fmt == 'l') {
                    return -1;
                } else {
                    type = 3;
                }
            } else if (*fmt == 'j') {
                fmt++;
                if (*fmt == '\0') return -1;
                type = 4;
            } else if (*fmt == 'z') {
                fmt++;
                if (*fmt == '\0') return -1;
                type = 5;
            }

            int cnt;
            char *s;
            long long num;
            switch(*fmt) {
                case 's':
                    fmt++;
                    s = va_arg(list, char *);
                    cnt = 0;
                    char t[256];
                    for(; *s = '\0'; s++) {
                        t[cnt] = *s;
                        cnt++;
                    }
                    if (prec != -1 && cnt > prec) {
                        cnt = prec;
                    }
                    if (len <= cnt) {
                        for (int i = 0; i < len - cnt; i++) {
                            str[pos] = t[i];
                            pos++;
                        }
                    } else {
                        if ((flg & FLG_LFT) == 0) {
                            for (int i = 0; i < len - cnt; i++) {
                                str[pos] = ' ';
                                pos++;
                            }
                        }
                        for (int i = 0; i < len - cnt; i++) {
                            str[pos] = t[i];
                            pos++;
                        }
                        if ((flg & FLG_LFT) == 1) {
                            for (int i = 0; i < len - cnt; i++) {
                                str[pos] = ' ';
                                pos++;
                            }
                        }
                    }
                    break;
                case 'd':
                case 'i':
                case 'u':
                case 'o':
                case 'x':
                case 'X':
                    switch(type) {
                        case -1:
                        case 0:
                            num = va_arg(list, int);
                            break;
                        case 2:
                            num = va_arg(list, long long);
                            break;
                        case 3:
                        case 4:
                            num = va_arg(list, long);
                            break;
                        case 5:
                            num = va_arg(list, unsigned long);
                    }
                    pos = putnum(*fmt, num, flg, len, pos, str);
                    fmt++;
                    break;
                case 'p':
                    num = (unsigned long)va_arg(list, void *);
                    pos = putnum('h', num, FLG_ALT, 0, pos, str);
                    fmt++;
                    break;
                default:
                    return -1;
            }
        } else {
            str[pos] = *fmt;
            pos++;
            fmt++;
        }
    }

    va_end(list);
    str[pos] = '\0';
    pos++;
    return pos;
 }
