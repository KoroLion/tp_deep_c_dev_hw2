/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <check.h>

#include "include/date_utils.h"

START_TEST(utils_test) {
    fail_unless(month_to_quarter(3) == 1, "month_to_quarter_test");
    fail_unless(month_to_quarter(4) == 2, "month_to_quarter_test");
    fail_unless(month_to_quarter(8) == 3, "month_to_quarter_test");
    fail_unless(month_to_quarter(12) == 4, "month_to_quarter_test");
    fail_unless(month_to_quarter(0) == -1, "month_to_quarter_test");
    fail_unless(month_to_quarter(13) == -1, "month_to_quarter_test");
} END_TEST

int main() {
    Suite *s1 = suite_create("Core");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_test(tc1_1, utils_test);

    srunner_run_all(sr, CK_ENV);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0? 0: 1;
}
