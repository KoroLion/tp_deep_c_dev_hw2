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

void random_data_string(char *s, int s_len, unsigned *rseed) {
    int y = 2000 + rand_r(rseed) % 21;

    char m[3], d[3];

    int v = rand_r(rseed) % 13;
    m[1] = v % 10 + 48;
    v /= 10;
    m[0] = v % 10 + 48;
    m[2] = 0;

    v = rand_r(rseed) % 29;
    d[1] = v % 10 + 48;
    v /= 10;
    d[0] = v % 10 + 48;
    d[2] = 0;

    float scr_avg = (rand_r(rseed) % 5001) / 1000.0;
    if (scr_avg <= 1.0000) {
        scr_avg = 0;
    }
    int scr_amnt = rand_r(rseed) % 10000;
    int scr_lst = rand_r(rseed) % 6;

    int c_wrtn = 0;
    if (scr_avg <= 1.0000) {
        c_wrtn = snprintf(s, s_len, "0 ");
    } else {
        c_wrtn = snprintf(s, s_len, "%0.2f ", scr_avg);
    }

    snprintf(s + c_wrtn, s_len - c_wrtn, "%d %d-%s-%s %d",
        scr_amnt,
        y, m, d,
        scr_lst);
}

int create_random_data_file(const char *fpath, int64_t amnt, unsigned *rseed) {
    FILE *f = fopen(fpath, "w");
    if (f == NULL) {
        return -1;
    }

    int s_len = 255;
    char *s = malloc(s_len * sizeof(s));
    for (int64_t i = 1; i <= amnt; i++) {
        s[0] = 0;
        random_data_string(s, s_len, rseed);
        fprintf(f, "%" PRId64 " %s\n", i, s);
    }
    free(s);

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

    const int buf_len = 1024;
    char *buf = malloc(buf_len * sizeof(buf));
    struct comment_data c;
    int filtered_amount = 0;

    while (fgets(buf, buf_len, f) != NULL) {
        int res = sscanf(buf, "%d %f %d %d-%d-%d %d\n",
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

        if (c.score_avg > 4.0000) {
            printf("%s", buf);
            filtered_amount++;
        }
    }

    free(buf);
    fclose(f);

    return filtered_amount;
}


int main(int argc, char **argv) {
    unsigned seed = time(0);
    create_random_data_file("test_data.txt", 100, &seed);

    int filtered_amount = count_gt_year_from_file("test_data.txt", 2015);
    printf("There are %d comments which average score is more than 4.\n",
        filtered_amount);

    return 0;
}
