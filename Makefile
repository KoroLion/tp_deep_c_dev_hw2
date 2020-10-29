# Copyright 2020 KoroLion (github.com/KoroLion)

# $@ - name of target

VPATH:=src
SHELL:=/bin/bash

CC:=gcc
CFLAGS:=-Wall -Werror -Wpedantic
SRC_DIR:=src
BUILD_DIR:=src
UNAME_S:=$(shell uname -s)

all: main_sequential.out main_parallel.out randgen.out

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $(SRC_DIR)/$@
libcomment_data_parallel.so: comment_data_parallel.c comment_data.c date_utils.c
	cd $(SRC_DIR) && $(CC) -fPIC -c comment_data.c comment_data_parallel.c date_utils.c $(CFLAGS)
	cd $(SRC_DIR) && $(CC) -shared -o $@ comment_data.o date_utils.o comment_data_parallel.o -lpthread

libcomment_data.a: comment_data_sequential.o comment_data.o date_utils.o
	cd $(SRC_DIR) && ar -rc $@ comment_data_sequential.o comment_data.o date_utils.o
librandom_data_gen.a: random_data_gen.o date_utils.o comment_data_sequential.o
	cd $(SRC_DIR) && ar -rc $@ random_data_gen.o date_utils.o
randgen.out: randgen.c librandom_data_gen.a
	cd $(SRC_DIR) && $(CC) -o $@ randgen.c -L'.' -l'random_data_gen' $(CFLAGS)

# dynamic library for paralell alg
main_parallel.out: main.c libcomment_data_parallel.so
	cd $(SRC_DIR) && $(CC) -o $@ main.c -L'.' -l'comment_data_parallel' -lpthread $(CFLAGS)
# static library for sequential alg
main_sequential.out: main.c libcomment_data.a
	cd $(SRC_DIR) && $(CC) -o $@ main.c -L'.' -l'comment_data' -lpthread $(CFLAGS)

generate: randgen.out
	cd $(SRC_DIR) && ./randgen.out test_data.txt 10000000
run_seq: main_sequential.out
	cd $(SRC_DIR) && ./main_sequential.out test_data.txt
run_par: main_parallel.out
	cd $(SRC_DIR) && LD_LIBRARY_PATH=. ./main_parallel.out test_data.txt

test_sequential.out: test.c
ifneq ($(UNAME_S),Darwin)
	cd $(SRC_DIR) && $(CC) test.c comment_data.c comment_data_sequential.c date_utils.c -o $@ -lcheck -lm -lpthread -lrt -lsubunit -fprofile-arcs -ftest-coverage $(CFLAGS)
endif
test_parallel.out: comment_data_parallel.c comment_data.c date_utils.c test.c
ifneq ($(UNAME_S),Darwin)
	cd $(SRC_DIR) && $(CC) test.c comment_data.c comment_data_parallel.c date_utils.c -o $@ -lcheck -lm -lpthread -lrt -lsubunit -fprofile-arcs -ftest-coverage $(CFLAGS)
endif
stress_test: randgen.out main_parallel.out main_sequential.out
	@echo '----- Stress test 1 (light) -----'
	cd $(SRC_DIR) && ./randgen.out test_data_100000.txt 100000
	cd $(SRC_DIR) && time ./main_sequential.out test_data_100000.txt
	cd $(SRC_DIR) && time LD_LIBRARY_PATH=. ./main_parallel.out test_data_100000.txt
	@echo -----
	@echo '----- Stress test 2 (med) -----'
	-@cd $(SRC_DIR) && test ! -f test_data_1000000.txt && ./randgen.out test_data_1000000.txt 1000000
	cd $(SRC_DIR) && time ./main_sequential.out test_data_1000000.txt
	cd $(SRC_DIR) && time LD_LIBRARY_PATH=. ./main_parallel.out test_data_1000000.txt
	@echo -----
	@echo '----- Stress test 3 (heavy) -----'
	-@cd $(SRC_DIR) && test ! -f test_data_20000000.txt && ./randgen.out test_data_20000000.txt 20000000
	cd $(SRC_DIR) && time ./main_sequential.out test_data_20000000.txt
	cd $(SRC_DIR) && time LD_LIBRARY_PATH=. ./main_parallel.out test_data_20000000.txt
	@echo -----
test: test_sequential.out test_parallel.out main_sequential.out main_parallel.out randgen.out
	python3 -m cpplint --filter=-readability/casting $(SRC_DIR)/*.c $(SRC_DIR)/include/*.h
	cppcheck --error-exitcode=1 $(SRC_DIR)/*.c $(SRC_DIR)/include/*.h
# valgrind and check does not work under macOS
ifneq ($(UNAME_S),Darwin)
	cd $(SRC_DIR) && ./test_sequential.out
	cd $(SRC_DIR) && ./test_parallel.out
	cd $(SRC_DIR) && valgrind --leak-check=yes --error-exitcode=1 ./randgen.out test_data.txt 10000
	cd $(SRC_DIR) && valgrind --leak-check=yes --error-exitcode=1 ./main_sequential.out test_data.txt
	cd $(SRC_DIR) && valgrind --leak-check=yes --error-exitcode=1 env LD_LIBRARY_PATH=. ./main_parallel.out test_data.txt
	cd $(SRC_DIR) && gcov date_utils.c comment_data.c comment_data_sequential.c comment_data_parallel.c
else
	cd $(SRC_DIR) && ./randgen.out test_data.txt 100000
	cd $(SRC_DIR) && ./main_sequential.out
	cd $(SRC_DIR) && ./main_parallel.out
endif
clean_stress:
	 cd $(SRC_DIR) &&& rm -f *.txt
clean:
	cd $(SRC_DIR) && rm -f *.o *.gcno *.gcda *.out *.gcov *.exe *.a *.so
