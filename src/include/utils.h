/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/
#ifndef SRC_INCLUDE_UTILS_H_
#define SRC_INCLUDE_UTILS_H_

#define debug_print(fmt, ...) \
            do { if (DEBUG) { fprintf(stderr, fmt, __VA_ARGS__); } } while (0)

struct date {
    int d, m, y;
};

struct date get_current_date();
int month_to_quarter(int m);

#endif  // SRC_INCLUDE_UTILS_H_
