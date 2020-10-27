/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/
#include <stdio.h>

#include "include/comment_data.h"

int main() {
    printf("Parallel lines filtered: %d\n",
        count_actual_commnents_parallel("test_data.txt", 4));
    printf("Default lines filtered: %d\n",
        count_actual_comments("test_data.txt", 4));

    return 0;
}
