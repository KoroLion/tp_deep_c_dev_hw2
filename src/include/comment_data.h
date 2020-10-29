/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/
#ifndef SRC_INCLUDE_COMMENT_DATA_H_
#define SRC_INCLUDE_COMMENT_DATA_H_

#include <stdbool.h>
#include <inttypes.h>

#include "date_utils.h"  // NOLINT

struct comment_data {
    int id;
    float score_average;
    int score_amount;
    struct date ld;
    int score_last;
};

void free_arr(void **ptr, int length);
int read_file(char ***p_str_arr, const char *fpath);

bool parse_comment(struct comment_data *c, char *s);
bool is_comment_in_last_q(const struct comment_data c);
int count_actual_comments(const char *fpath, int avg_score);

#endif  // SRC_INCLUDE_COMMENT_DATA_H_
