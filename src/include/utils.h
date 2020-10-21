/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/
#ifndef SRC_INCLUDE_UTILS_H_
#define SRC_INCLUDE_UTILS_H_

struct date {
    int d, m, y;
};

struct date get_current_date();
int month_to_quarter(int m);
void to_date_format(int v, char *s);

#endif  // SRC_INCLUDE_UTILS_H_
