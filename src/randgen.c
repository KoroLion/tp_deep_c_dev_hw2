/*
Вариант #66
В вашем распоряжении — массив из 10 млн. структур, содержащих числовой идентификатор записи (комментария) в социальной сети, а также поля с семантикой средней оценки (целое 0 или вещественное от 1,0 до 5,0), числа поданных голосов (неотрицательное целое), даты выставления последней оценки и ее значения (целое от 1 до 5). Составьте наивный алгоритм подсчета записей с оценкой не ниже 4,0, получивших хотя бы одну оценку за последний квартал, а затем реализуйте параллельный алгоритм с использованием нескольких потоков с учетом возможной декомпозиции (разбиения) структуры и выравнивания результатов по линиям кэш-памяти.
*/
/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "include/comment_data.h"

const int DEFAULT_AMOUNT = 100;

int main(int argc, char **argv) {
    int amount = DEFAULT_AMOUNT;
    if (argc == 1) {
        printf("Usage: ./<bin> <file_path> <rand_etries_amount=%d>\n",
            amount);
        return 0;
    }

    if (argc > 2) {
        int64_t k = strtoll(argv[2], NULL, 10);
        if (k > 0) {
            amount = k;
        } else {
            printf("Warning: incorrect arg for amount!\n");
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
