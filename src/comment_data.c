/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#include "include/date_utils.h"
#include "include/comment_data.h"

int count_last_good_comments_from_file(char *fpath) {
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
    struct comment_data c;
    int filtered_amount = 0;

    while (fgets(buf, buf_len, f) != NULL) {
        int res = sscanf(buf, "%d %f %d %d-%d-%d %d\n",
            &c.id, &c.score_average,
            &c.score_amount,
            &c.ly, &c.lm,
            &c.ld,
            &c.score_last);
        if (res == 0) {
            printf("Error: while reading %s\n", fpath);
            free(buf);
            fclose(f);
            return -3;
        }

        int upd_q = month_to_quarter(c.lm);
        struct date cur_date = get_current_date();
        int last_q = month_to_quarter(cur_date.m) - 1;
        int q_y = cur_date.y;
        if (last_q == 0) {
            last_q = 4;
            q_y--;
        }
        bool is_comment_in_current_q = last_q == upd_q && q_y == c.ly;
        if (is_comment_in_current_q && c.score_average > 4.0000) {
            filtered_amount++;
        }
    }

    free(buf);
    fclose(f);

    return filtered_amount;
}
