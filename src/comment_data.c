/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pthread.h>
#ifdef __APPLE__
    #define get_nprocs() 4
#else
    #include <sys/sysinfo.h>
#endif

#include "include/date_utils.h"
#include "include/comment_data.h"

bool parse_comment(struct comment_data *c, char *s) {
    int var_amount = sscanf(s, "%d %f %d %d-%d-%d %d\n",
        &c->id, &c->score_average,
        &c->score_amount,
        &c->ly, &c->lm, &c->ld,
        &c->score_last);

    return var_amount == 7;
}

bool is_comment_in_last_q(const struct comment_data c) {
    int upd_q = month_to_quarter(c.lm);
    struct date cur_date = get_current_date();
    int last_q = month_to_quarter(cur_date.m) - 1;
    int q_y = cur_date.y;
    if (last_q == 0) {
        last_q = 4;
        q_y--;
    }

    return last_q == upd_q && q_y == c.ly;
}

int _count_actual_comments_woffset(
        const char *fpath, float avg_score, int32_t offset, int32_t amount) {
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

    int skipped_amount = 0;
    char ch;
    while (skipped_amount < offset && (ch = fgetc(f)) != EOF) {
        if (ch == '\n') {
            skipped_amount++;
        }
    }

    int filtered_amount = 0, checked_amount = 0;
    struct comment_data *c = malloc(sizeof(*c));
    while (fgets(buf, buf_len, f) != NULL
            && (checked_amount < amount || amount == 0)) {
        if (parse_comment(c, buf) == false) {
            printf("Error: while reading %s\n", fpath);
            free(c);
            free(buf);
            fclose(f);
            return -3;
        }

        if (is_comment_in_last_q(*c) && c->score_average > avg_score) {
            filtered_amount++;
        }
        checked_amount++;
    }

    free(c);
    free(buf);
    fclose(f);

    return filtered_amount;
}
