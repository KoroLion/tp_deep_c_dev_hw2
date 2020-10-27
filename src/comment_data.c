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
        const char *fpath, float avg_score, int offset, int amount) {
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

int count_actual_comments(const char *fpath, int avg_score) {
    return _count_actual_comments_woffset(fpath, avg_score, 0, 0);
}

int count_lines(const char *fpath) {
    FILE *fp = fopen("test_data.txt", "r");
    char c;
    int ln_amount = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            ln_amount++;
        }
    }
    fclose(fp);

    return ln_amount;
}

struct args {
    const char *fpath;
    int avg_score;
    int ln_offset, ln_amount;
};

void *thread_count_comments(void *arg) {
    struct args *a = arg;

    FILE *fp = fopen(a->fpath, "r");
    int flt_amnt = _count_actual_comments_woffset(
        a->fpath, a->avg_score, a->ln_offset, a->ln_amount);

    fclose(fp);

    pthread_exit((void*)(uintptr_t)flt_amnt);
}

int count_actual_commnents_parallel(const char *fpath, int avg_score) {
    int ln_amount = count_lines(fpath),
        nproc = get_nprocs(),
        ln_per_thread = ln_amount / nproc,
        ln_remains = ln_amount % nproc,
        flt_amount = 0;

    int errflag;
    pthread_t *threads = malloc(nproc * sizeof(*threads));
    struct args *a[nproc];

    for (int i = 0; i < nproc; i++) {
        a[i] = malloc(sizeof(*a[i]));
        a[i]->fpath = fpath;
        a[i]->avg_score = avg_score;
        a[i]->ln_offset = ln_per_thread * i;
        a[i]->ln_amount = ln_per_thread;
        if (i + 1 == nproc) {
            a[i]->ln_amount += ln_remains;
        }

        errflag = pthread_create(
            &threads[i], NULL, thread_count_comments, a[i]);
        if (errflag != 0) {
            free(threads);
            return -4;
        }
    }

    for (int i = 0; i < nproc; i++) {
        void *res = NULL;
        if (pthread_join(threads[i], &res)) {
            free(threads);
            return -4;
        }
        flt_amount += (int)res;
    }
    free(threads);

    return flt_amount;
}
