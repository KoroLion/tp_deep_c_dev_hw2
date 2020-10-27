/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/
#include <stdio.h>

#include "include/comment_data.h"

int main(int argc, char **argv) {
    int flt_amnt = count_actual_comments_parallel("test_data.txt", 4);
    if (flt_amnt < 0) {
        printf("Error: while reading data file!\n");
        return 1;
    }
    printf("%d comments are good and actual.\n", flt_amnt);

    return 0;
}
