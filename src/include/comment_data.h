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

void random_data(struct comment_data *c, int rand_years, unsigned *rseed);
int random_data_string(char *s, int s_len, unsigned *rseed);
int create_random_data_file(const char *fpath, int64_t amnt, unsigned *rseed);
int count_last_good_comments_from_file(char *fpath);

#endif  // SRC_INCLUDE_COMMENT_DATA_H_
