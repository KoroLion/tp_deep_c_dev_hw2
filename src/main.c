/*
Вариант #66
В вашем распоряжении — массив из 10 млн. структур, содержащих числовой идентификатор записи (комментария) в социальной сети, а также поля с семантикой средней оценки (целое 0 или вещественное от 1,0 до 5,0), числа поданных голосов (неотрицательное целое), даты выставления последней оценки и ее значения (целое от 1 до 5). Составьте наивный алгоритм подсчета записей с оценкой не ниже 4,0, получивших хотя бы одну оценку за последний квартал, а затем реализуйте параллельный алгоритм с использованием нескольких потоков с учетом возможной декомпозиции (разбиения) структуры и выравнивания результатов по линиям кэш-памяти.
*/
/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/

#include <stdio.h>
#include <time.h>

#include "include/comment_data.h"

int main(int argc, char **argv) {
    int filtered_amount = count_last_good_comments_from_file("test_data.txt");
    if (filtered_amount < 0) {
        printf("Error: while reading data file!");
        return 1;
    }

    printf("There're %d comments from the last quarter with score > 4.\n",
        filtered_amount);

    return 0;
}
