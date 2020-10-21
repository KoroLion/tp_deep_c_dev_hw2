/*
Вариант #66
В вашем распоряжении — массив из 10 млн. структур, содержащих числовой идентификатор записи (комментария) в социальной сети, а также поля с семантикой средней оценки (целое 0 или вещественное от 1,0 до 5,0), числа поданных голосов (неотрицательное целое), даты выставления последней оценки и ее значения (целое от 1 до 5). Составьте наивный алгоритм подсчета записей с оценкой не ниже 4,0, получивших хотя бы одну оценку за последний квартал, а затем реализуйте параллельный алгоритм с использованием нескольких потоков с учетом возможной декомпозиции (разбиения) структуры и выравнивания результатов по линиям кэш-памяти.
*/
/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include <unistd.h>

void random_data_string(char *s) {
    int y = 2000 + rand() % 21;

    char m[3], d[3];

    int v = rand() % 13;
    m[1] = v % 10 + 48;
    v /= 10;
    m[0] = v % 10 + 48;
    m[2] = 0;

    v = rand() % 29;
    d[1] = v % 10 + 48;
    v /= 10;
    d[0] = v % 10 + 48;
    d[2] = 0;

    float scr_avg = rand() % 6;
    int scr_amnt = rand() % 10000;
    int scr_lst = rand() % 6;

    snprintf(s, sizeof(s) * 255, "%f %d %d-%s-%s %d",
        scr_avg,
        scr_amnt,
        y, m, d,
        scr_lst);
}

int create_random_data_file(const char *fpath, int64_t amount) {
    FILE *f = fopen(fpath, "w");
    if (f == NULL) {
        return -1;
    }

    for (int64_t i = 1; i <= amount; i++) {
        char *s = malloc(255 * sizeof(s));
        s[0] = 0;
        random_data_string(s);
        fprintf(f, "%" PRId64 " %s\n", i, s);
        free(s);
    }

    fclose(f);
    return 0;
}

struct comment_data {
    int id;
    float score_avg;
    int score_amount;
    int ly, lm, ld;
    int score_last;
};

int count_gt_year_from_file(char *fpath, int year) {
    FILE *f = fopen(fpath, "r");
    if (f == NULL) {
        return -1;
    }
    char s[1024];

    struct comment_data c;
    while (fgets(s, 1024, f) != NULL) {
        int res = sscanf(s, "%d %f %d %d-%d-%d %d\n",
            &c.id, &c.score_avg,
            &c.score_amount,
            &c.ly, &c.lm,
            &c.ld,
            &c.score_last);
        if (res == 0) {
            printf("Error: while reading %s\n", fpath);
            fclose(f);
            return -3;
        }

        if (c.ly > year) {
            printf("%d %d\n", c.id, c.ly);
        }
    }

    fclose(f);
    return 0;
}


int main(int argc, char **argv) {
    srand(time(0));

    create_random_data_file("test_data.txt", 100);
    count_gt_year_from_file("test_data.txt", 2015);

    return 0;
}
