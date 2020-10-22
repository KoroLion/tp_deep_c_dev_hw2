/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "include/comment_data.h"
#include "include/random_data_gen.h"

const int DEFAULT_AMOUNT = 100;

int main(int argc, char **argv) {
    int amount = DEFAULT_AMOUNT;
    if (argc == 1) {
        printf("Usage: ./<bin> <file_path> <rand_entries_amount=%d>\n",
            amount);
        return 0;
    }

    if (argc > 2) {
        int64_t k = strtoll(argv[2], NULL, 10);
        if (k > 0) {
            amount = k;
        } else {
            printf("Error: incorrect arg for amount!\n");
            return 1;
        }
    }

    unsigned seed = time(0);
    if (create_random_data_file(argv[1], amount, &seed)) {
        printf("Error: While creating data file!\n");
        return 1;
    }

    printf("Created %d comments entries in %s.\n", amount, argv[1]);

    return 0;
}
