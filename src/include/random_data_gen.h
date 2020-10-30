/*
Copyright 2020 KoroLion (github.com/KoroLion)
*/
#ifndef SRC_INCLUDE_RANDOM_DATA_GEN_H_
#define SRC_INCLUDE_RANDOM_DATA_GEN_H_

#include <stdint.h>

void random_data(struct comment_data *c, int rand_years, unsigned *rseed);
int random_data_string(char *s, int s_len, unsigned *rseed);
int create_random_data_file(const char *fpath, int64_t amnt, unsigned *rseed);

#endif  // SRC_INCLUDE_RANDOM_DATA_GEN_H_
