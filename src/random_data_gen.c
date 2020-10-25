/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "include/date_utils.h"
#include "include/comment_data.h"
#include "include/random_data_gen.h"

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
        char buf[255];
        struct date d;
        d.y = c.ly; d.m = c.lm; d.d = c.ld;
        format_date(buf, d);
        snprintf(s, s_len,
            "%0.2f %d %s %d",
            c.score_average,
            c.score_amount,
            buf,
            c.score_last);
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
