/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/

#include <stdio.h>

#include "include/comment_data.h"

int count_actual_comments(const char *fpath, int avg_score) {
    return _count_actual_comments_woffset(fpath, avg_score, 0, 0);
}
