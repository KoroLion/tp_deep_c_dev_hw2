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

const int BASE_LINES_AMOUNT = 1000000;
const int MAX_LINE_LEN = 40;

void free_arr(void **ptr, int length) {
    for (int i = 0; i < length; i++) {
        if (ptr[i] != NULL) {
            free(ptr[i]);
        }
    }
}

int read_file(char ***p_str_arr, const char *fpath) {
    int current_size = BASE_LINES_AMOUNT;
    char **c_data = *p_str_arr;
    c_data = malloc(current_size * sizeof(*c_data));

    FILE *fp = fopen(fpath, "r");

    int32_t ln_amount = 0;
    int s_len = MAX_LINE_LEN;
    char *res;
    do {
        if (ln_amount >= current_size) {
            current_size *= 2;
            char **new_ptr = realloc(c_data, current_size * sizeof(*c_data));
            if (new_ptr == NULL) {
                free_arr((void**)c_data, ln_amount);
                free(c_data);
                return -2;
            }
            c_data = new_ptr;
        }
        c_data[ln_amount] = malloc(s_len * sizeof(**c_data));
        if (c_data[ln_amount] == NULL) {
            free_arr((void**)c_data, ln_amount);
            free(c_data);
            return -2;
        }
        res = fgets(c_data[ln_amount], s_len, fp);
        ln_amount++;
    } while (res != NULL);
    ln_amount--;
    free(c_data[ln_amount]);  // the last el is NULL
    fclose(fp);

    *p_str_arr = c_data;

    return ln_amount;
}

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
