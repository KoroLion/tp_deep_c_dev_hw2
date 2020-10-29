/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/

#include <stdio.h>
#include <stdlib.h>

#include "include/comment_data.h"

int count_actual_comments(const char *fpath, int avg_score) {
    char **c_data;
    int ln_amount = read_file(&c_data, fpath);
    if (ln_amount <= 0) {
        return -3;
    }

    int filtered_amount = 0;
    struct comment_data *c = malloc(sizeof(*c));
    int i = 0;
    while (i < ln_amount) {
        if (!parse_comment(c, c_data[i])) {
            free_arr((void**)c_data, ln_amount);
            free(c_data);
            return -3;
        }

        if (is_comment_in_last_q(*c) && c->score_average > avg_score) {
            filtered_amount++;
        }
        i++;
    }

    free(c);
    free_arr((void**)c_data, ln_amount);
    free(c_data);

    return filtered_amount;
}
