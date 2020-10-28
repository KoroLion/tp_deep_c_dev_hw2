/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include <check.h>

#include "include/date_utils.h"
#include "include/comment_data.h"

const int TEST_BUF_SIZE = 1024;

bool create_file(const char *fname, char *data) {
    FILE *pf = fopen(fname, "wr");
    if (pf == NULL) {
        return false;
    }
    fputs(data, pf);
    fclose(pf);
    return true;
}

START_TEST(date_utils_test) {
    fail_unless(month_to_quarter(3) == 1, "month_to_quarter_test");
    fail_unless(month_to_quarter(4) == 2, "month_to_quarter_test");
    fail_unless(month_to_quarter(8) == 3, "month_to_quarter_test");
    fail_unless(month_to_quarter(12) == 4, "month_to_quarter_test");
    fail_unless(month_to_quarter(0) == -1, "month_to_quarter_test");
    fail_unless(month_to_quarter(13) == -1, "month_to_quarter_test");

    time_t t = time(NULL);
    struct tm buf;
    struct tm dt = *localtime_r(&t, &buf);
    struct date d = get_current_date();
    fail_unless(d.y == (dt.tm_year + 1900), "cur_date");
    fail_unless(d.m == (dt.tm_mon + 1), "cur_date");
    fail_unless(d.d == (dt.tm_mday), "cur_date");

    int a = 1, b = 12;
    char *s = malloc(3 * sizeof(*s));
    to_date_format(a, s);
    fail_unless(s[0] == '0' && s[1] == '1' && s[2] == 0, "to_d_form");
    to_date_format(b, s);
    fail_unless(s[0] == '1' && s[1] == '2' && s[2] == 0, "to_d_form");
} END_TEST

START_TEST(comment_data_test) {
    char *ok_comment_s = "9 2.56 9022 2020-06-21 1";
    char *bad_comment_s = "9 WOLF";
    struct comment_data *c = malloc(sizeof(*c));
    fail_unless(parse_comment(c, ok_comment_s) == true, "parse_comment");
    fail_unless(c->id == 9, "parse_comment");
    fail_unless(abs(c->score_average - 2.56) < 0.01, "parse_comment");
    fail_unless(c->score_amount == 9022, "parse_comment");
    fail_unless(c->ld.y == 2020 && c->ld.m == 6 && c->ld.d == 21,
        "parse_comment");
    fail_unless(c->score_last == 1, "parse_comment");
    fail_unless(parse_comment(c, bad_comment_s) == false, "parse_comment");

    struct date d = get_current_date();
    c->ld.y = d.y;
    c->ld.m = d.m;
    fail_unless(is_comment_in_last_q(*c) == false, "is_last_q");
    c->ld.m -= 3;
    if (c->ld.m <= 0) {
        c->ld.m = 12;
    }
    fail_unless(is_comment_in_last_q(*c) == true, "is_last_q");

    struct date d_last_q, d_cur_q = get_current_date();
    int last_q_m = d_cur_q.m - 3,
        last_q_y = d_cur_q.y;
    if (last_q_m <= 0) {
        last_q_m = 12;
        last_q_y -= 1;
    }
    d_last_q.y = last_q_y;
    d_last_q.m = last_q_m;
    d_last_q.d = 1;
    char last_q_sd[32], cur_q_sd[32];
    format_date(last_q_sd, d_last_q);
    format_date(cur_q_sd, d_cur_q);

    const char *fname = "temp_file.txt";
    char data[TEST_BUF_SIZE];

    snprintf(data, TEST_BUF_SIZE * sizeof(*data),
        "9 2.56 9022 %s 1\n"  // -
        "9 4.52 9022 %s 1\n"  // +
        "9 4.52 9022 %s 1\n",  // -
        last_q_sd, last_q_sd, cur_q_sd);
    create_file(fname, data);
    fail_unless(count_actual_comments(fname, 4) == 1, "count");

    snprintf(data, TEST_BUF_SIZE * sizeof(*data), "bad_data");
    create_file(fname, data);
    fail_unless(count_actual_comments(fname, 4) == -3, "count");

    snprintf(data, TEST_BUF_SIZE,
        "1 2.56 9022 %s 1\n"  // -
        "2 4.52 9022 %s 1\n"  // +
        "3 4.52 9022 %s 1\n"  // -
        "4 5 41234 2002-02-01 5\n"  // -
        "5 4.31 1233 %s 4\n"  // +
        "6 4.01 321 %s 2\n"   // +
        "7 3.99 321 %s 5\n",   // -
        last_q_sd, last_q_sd, cur_q_sd, last_q_sd, last_q_sd, last_q_sd);
    create_file(fname, data);
    fail_unless(count_actual_comments(fname, 4) == 3, "count");

    unlink(fname);
} END_TEST

int main() {
    Suite *s1 = suite_create("Core");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_test(tc1_1, date_utils_test);
    tcase_add_test(tc1_1, comment_data_test);

    srunner_run_all(sr, CK_ENV);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0? 0: 1;
}
