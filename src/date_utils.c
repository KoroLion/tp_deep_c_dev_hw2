/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/

#include <time.h>

#include "include/date_utils.h"

int to_string(int v, char *s) {
    int len = 0;
    int a[32];
    while (v > 0) {
        a[len++] = v % 10;
        v /= 10;
    }
    for (int i = len - 1; i >= 0; i--) {
        s[len - i - 1] = a[i] + 48;
    }
    s[len] = 0;

    return --len;
}

struct date get_current_date() {
    time_t t = time(NULL);
    struct tm buf;
    struct tm dt = *localtime_r(&t, &buf);
    struct date d;
    d.d = dt.tm_mday;
    d.m = dt.tm_mon + 1;  // tm_mon is in range [0; 11]
    d.y = dt.tm_year + 1900;  // 0 = year 1900
    return d;
}

int month_to_quarter(int m) {
    // quarters: q1: 1, 2, 3; q2: 4, 5, 6; q3: 7, 8, 9; q4: 10, 11, 12
    if (m <= 0) {
        return -1;
    } else if (m <= 3) {
        return 1;
    } else if (m <= 6) {
        return 2;
    } else if (m <= 9) {
        return 3;
    } else if (m <= 12) {
        return 4;
    } else {
        return -1;
    }
}

void format_date(char *s, const struct date d) {
    char buf[3];
    int len = to_string(d.y, s);
    s[len + 1] = '-';
    to_date_format(d.m, buf);
    s[len + 2] = buf[0];
    s[len + 3] = buf[1];
    s[len + 4] = '-';
    to_date_format(d.d, buf);
    s[len + 5] = buf[0];
    s[len + 6] = buf[1];
    s[len + 7] = 0;
}

void to_date_format(int v, char *s) {
    // 1 <= v <= 99
    // prepends zero if needed and converts to string
    s[1] = v % 10 + 48;
    v /= 10;
    s[0] = v % 10 + 48;
    s[2] = 0;
}
