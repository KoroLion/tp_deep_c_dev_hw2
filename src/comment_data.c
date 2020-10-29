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
        &c->ld.y, &c->ld.m, &c->ld.d,
        &c->score_last);

    return var_amount == 7;
}

bool is_comment_in_last_q(const struct comment_data c) {
    int upd_q = month_to_quarter(c.ld.m);
    struct date cur_date = get_current_date();
    int last_q = month_to_quarter(cur_date.m) - 1;
    int q_y = cur_date.y;
    if (last_q == 0) {
        last_q = 4;
        q_y--;
    }

    return last_q == upd_q && q_y == c.ld.y;
}
