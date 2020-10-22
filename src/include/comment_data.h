/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/
#ifndef SRC_INCLUDE_COMMENT_DATA_H_
#define SRC_INCLUDE_COMMENT_DATA_H_

struct comment_data {
    int id;
    float score_average;
    int score_amount;
    int ly, lm, ld;
    int score_last;
};

int count_last_good_comments_from_file(char *fpath);

#endif  // SRC_INCLUDE_COMMENT_DATA_H_
