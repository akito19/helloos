#include "bootpack.h"

// https://wisteria0410ss.hatenablog.com/entry/2019/02/21/151455
int strcmp(const char *s1, const char *s2)
{
    int i;
    for (i = 0;; i++) {
        if (s1[i] > s2[i]) return 1;
        if (s1[i] < s2[i]) return -1;
        if (s1[i] == 0 && s2[i] == 0) return 0;
    }
}
