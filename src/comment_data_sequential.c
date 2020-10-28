/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/

#include <stdio.h>
#include <stdlib.h>

#include "include/comment_data.h"

int count_actual_comments(const char *fpath, int avg_score) {
    FILE *f = fopen(fpath, "r");
    if (f == NULL) {
        return -1;
    }

    const int buf_len = 1024;
    char *buf = malloc(buf_len * sizeof(buf));
    if (buf == NULL) {
        fclose(f);
        return -2;
    }

    int filtered_amount = 0;
    struct comment_data *c = malloc(sizeof(*c));
    while (fgets(buf, buf_len, f) != NULL) {
        if (!parse_comment(c, buf)) {
            free(c);
            free(buf);
            fclose(f);
            return -3;
        }

        if (is_comment_in_last_q(*c) && c->score_average > avg_score) {
            filtered_amount++;
        }
    }

    free(c);
    free(buf);
    fclose(f);

    return filtered_amount;
}
