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

struct args {
    char **c_data;
    int avg_score;
    int32_t ln_offset, ln_amount;
};

void *thread_count_comments(void *arg) {
    struct args *a = arg;

    int flt_amount = 0;
    int i = a->ln_offset;
    struct comment_data *c = malloc(sizeof(*c));
    if (c == NULL) {
        pthread_exit((void*)(uintptr_t)-2);
    }
    while (i < a->ln_offset + a->ln_amount) {
        if (!parse_comment(c, a->c_data[i])) {
            free(c);
            pthread_exit((void*)(uintptr_t)-3);
        }

        if (is_comment_in_last_q(*c) && c->score_average > a->avg_score) {
            flt_amount++;
        }
        free(a->c_data[i]);  // to free memory as soon as possible
        a->c_data[i] = NULL;  // to prevent free() of freed memory
        i++;
    }
    free(c);

    pthread_exit((void*)(uintptr_t)flt_amount);
}

int count_actual_comments(const char *fpath, int avg_score) {
    char **c_data;
    int ln_amount = read_file(&c_data, fpath);
    if (ln_amount <= 0) {
        return -3;
    }

    int nthreads = get_nprocs();
    int32_t ln_per_thread = ln_amount / nthreads,
        ln_remains = ln_amount % nthreads,
        flt_amount = 0;

    int errflag;
    pthread_t *threads = malloc(nthreads * sizeof(*threads));
    struct args *a[nthreads];

    for (int i = 0; i < nthreads; i++) {
        a[i] = malloc(sizeof(*a[i]));
        a[i]->c_data = c_data;
        a[i]->avg_score = avg_score;
        a[i]->ln_offset = ln_per_thread * i;
        a[i]->ln_amount = ln_per_thread;
        if (i + 1 == nthreads) {
            a[i]->ln_amount += ln_remains;
        }

        errflag = pthread_create(
            &threads[i], NULL, thread_count_comments, a[i]);
        if (errflag != 0) {
            free(threads);
            free_arr((void**)c_data, ln_amount);
            free(c_data);
            free_arr((void**)a, nthreads);
            return -4;
        }
    }

    for (int i = 0; i < nthreads; i++) {
        void *res = NULL;
        int ret = pthread_join(threads[i], &res);
        int32_t ln_amnt = (uintptr_t)res;
        if (ret || ln_amnt < 0) {
            free(threads);
            free_arr((void**)c_data, ln_amount);
            free(c_data);
            free_arr((void**)a, nthreads);
            return -3;
        }
        flt_amount += ln_amnt;
        free(a[i]);
        a[i] = NULL;
    }
    free(threads);
    free(c_data);  // elements of an array alreay freed inside threads

    return flt_amount;
}
