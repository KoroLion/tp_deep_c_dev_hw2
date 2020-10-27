/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/
#ifndef SRC_INCLUDE_COMMENT_DATA_H_
#define SRC_INCLUDE_COMMENT_DATA_H_

#include <stdbool.h>

struct comment_data {
    int id;
    float score_average;
    int score_amount;
    int ly, lm, ld;
    int score_last;
};

bool parse_comment(struct comment_data *c, char *s);
bool is_comment_in_last_q(const struct comment_data c);

int _count_actual_comments_woffset(
    const char *fpath, float avg_score, int offset, int amount);
int count_actual_comments(const char *fpath, int avg_score);
int count_actual_comments_parallel(const char *fpath, int avg_score);

#endif  // SRC_INCLUDE_COMMENT_DATA_H_
