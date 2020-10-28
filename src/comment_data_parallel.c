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

int count_lines(const char *fpath) {
    FILE *fp = fopen(fpath, "r");
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

int count_actual_comments(const char *fpath, int avg_score) {
    int nthreads = get_nprocs();
    int32_t ln_amount = count_lines(fpath),
        ln_per_thread = ln_amount / nthreads,
        ln_remains = ln_amount % nthreads,
        flt_amount = 0;

    int errflag;
    pthread_t *threads = malloc(nthreads * sizeof(*threads));
    struct args *a[nthreads];

    for (int i = 0; i < nthreads; i++) {
        a[i] = malloc(sizeof(*a[i]));
        a[i]->fpath = fpath;
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
            return -4;
        }
    }

    for (int i = 0; i < nthreads; i++) {
        void *res = NULL;
        if (pthread_join(threads[i], &res)) {
            free(threads);
            return -4;
        }
        flt_amount += (uintptr_t)res;
        free(a[i]);
    }
    free(threads);

    return flt_amount;
}
