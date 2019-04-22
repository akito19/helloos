
// https://wisteria0410ss.hatenablog.com/entry/2019/02/21/151455
#include "bootpack.h"

int strcmp(const char *s1, const char *s2)
{
    int i;
    for (i = 0;; i++) {
        if (s1[i] > s2[i]) return 1;
        if (s1[i] < s2[i]) return -1;
        if (s1[i] == 0 && s2[i] == 0) return 0;
    }
}

int starts_with(const char *s, const char *prefix) {
    int i;
    for(i = 0; prefix[i] != 0; i++) {
        if(s[i] != prefix[i]) return 0;
    }
    return 1;
}

// int strncmp(const char *s1, const char *s2, unsigned int length)
// {
//     int i;
//     for (i = 0; i <= length; i++) {
//         if (s1[i] > s2[i]) {
//             if (i > length) {
//                 return 0;
//             }
//             return 1;
//         }
//         if (s1[i] < s2[i]) {
//             if (i > length) {
//                 return 0;
//             }
//             return -1;
//         }
//         if (s1[i] == 0 && s2[i] == 0) return 0;
//     }
// }
