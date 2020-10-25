/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/
#ifndef SRC_INCLUDE_DATE_UTILS_H_
#define SRC_INCLUDE_DATE_UTILS_H_

struct date {
    int d, m, y;
};

int to_string(int v, char *s);
struct date get_current_date();
int month_to_quarter(int m);
void format_date(char *s, const struct date d);
void to_date_format(int v, char *s);

#endif  // SRC_INCLUDE_DATE_UTILS_H_
