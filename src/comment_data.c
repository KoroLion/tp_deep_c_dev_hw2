/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#include "include/utils.h"
#include "include/comment_data.h"

void random_data(struct comment_data *c, int rand_years, unsigned *rseed) {
    struct date cur_d = get_current_date();

    c->ly = (cur_d.y - rand_years) + rand_r(rseed) % (rand_years + 1);
    c->lm = rand_r(rseed) % 12 + 1;
    c->ld = rand_r(rseed) % 28 + 1;
    c->score_average = (rand_r(rseed) % 5001) / 1000.0;
    c->score_amount = rand_r(rseed) % 10000;
    c->score_last = rand_r(rseed) % 5 + 1;
}

int random_data_string(char *s, int s_len, unsigned *rseed) {
    struct comment_data c;
    random_data(&c, 2, rseed);

    if (c.score_average <= 1.0000) {
        snprintf(s, s_len, "0 0 00-00-0000 0");
    } else {
        char *sm = malloc(3 * sizeof(sm));
        if (sm == NULL) {
            return -2;
        }
        char *sd = malloc(3 * sizeof(sd));
        if (sd == NULL) {
            free(sm);
            return -2;
        }
        to_date_format(c.lm, sm);
        to_date_format(c.ld, sd);

        snprintf(s, s_len, "%0.2f %d %d-%s-%s %d",
            c.score_average,
            c.score_amount,
            c.ly, sm, sd,
            c.score_last);

        free(sm);
        free(sd);
    }

    return 0;
}

int create_random_data_file(const char *fpath, int64_t amnt, unsigned *rseed) {
    FILE *f = fopen(fpath, "w");
    if (f == NULL) {
        return -1;
    }

    int s_len = 255;
    char *s = malloc(s_len * sizeof(s));
    if (s == NULL) {
        fclose(f);
        return -2;
    }
    for (int64_t i = 1; i <= amnt; i++) {
        s[0] = 0;
        if (random_data_string(s, s_len, rseed)) {
            free(s);
            fclose(f);
            return -2;
        }
        fprintf(f, "%" PRId64 " %s\n", i, s);
    }

    free(s);
    fclose(f);

    return 0;
}

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
