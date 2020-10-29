# Copyright 2020 KoroLion (github.com/KoroLion)

# $@ - name of target
# .pio is a custom extension means position independent object

VPATH:=src:build:build/bin
SHELL:=/bin/bash

SRC_DIR:=./src
BUILD_DIR:=./build
BIN_DIR:=$(BUILD_DIR)/bin
TESTS_DIR:=$(BUILD_DIR)/tests

CC:=gcc
CFLAGS:=-Wall -Werror -Wpedantic
UNAME_S:=$(shell uname -s)

all: count_sequential.out count_parallel.out randgen_data.out

# build objects
%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -c -o $(BUILD_DIR)/$@
%.pio: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -fPIC -c -o $(BUILD_DIR)/$@

# build libs
librandom_data_gen.a: date_utils.o comment_data.o random_data_gen.o
	ar -rc $(BUILD_DIR)/$@ $(addprefix $(BUILD_DIR)/,$^)
libcomment_data_sequential.a: utils.o date_utils.o comment_data.o comment_data_sequential.o
	ar -rc $(BUILD_DIR)/$@ $(addprefix $(BUILD_DIR)/,$^)
libcomment_data_parallel.so: comment_data_parallel.pio comment_data.pio date_utils.pio utils.pio
	$(CC) -shared -o $(BIN_DIR)/$@ $(addprefix $(BUILD_DIR)/,$^) -lpthread

# build bins
# random comments data file generator
randgen_data.out: randgen.c librandom_data_gen.a
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $< -L$(BUILD_DIR) -l'random_data_gen'
# with static library for sequential alg
count_sequential.out: main.c libcomment_data_sequential.a
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $< -L$(BUILD_DIR) -l'comment_data_sequential'
# with dynamic library for paralell alg
count_parallel.out: main.c libcomment_data_parallel.so
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $< -L$(BIN_DIR) -l'comment_data_parallel' -lpthread

stress_test: randgen_data.out count_sequential.out count_parallel.out
	@echo '----- Stress test 1 (light) -----'
	$(BIN_DIR)/randgen_data.out $(BIN_DIR)/test_data_100000.txt 100000
	time $(BIN_DIR)/count_sequential.out $(BIN_DIR)/test_data_100000.txt
	time LD_LIBRARY_PATH=$(BIN_DIR) $(BIN_DIR)/count_parallel.out $(BIN_DIR)/test_data_100000.txt
	@echo -----
	@echo '----- Stress test 2 (med) -----'
	-test ! -f $(BIN_DIR)/test_data_1000000.txt && $(BIN_DIR)/randgen_data.out $(BIN_DIR)/test_data_1000000.txt 1000000
	time $(BIN_DIR)/count_sequential.out $(BIN_DIR)/test_data_1000000.txt
	time LD_LIBRARY_PATH=$(BIN_DIR) $(BIN_DIR)/count_parallel.out $(BIN_DIR)/test_data_1000000.txt
	@echo -----
	@echo '----- Stress test 3 (heavy) -----'
	-test ! -f $(BIN_DIR)/test_data_20000000.txt && $(BIN_DIR)/randgen_data.out $(BIN_DIR)/test_data_20000000.txt 20000000
	time $(BIN_DIR)/count_sequential.out $(BIN_DIR)/test_data_20000000.txt
	time LD_LIBRARY_PATH=$(BIN_DIR) $(BIN_DIR)/count_parallel.out $(BIN_DIR)/test_data_20000000.txt
	@echo -----

test_sequential.out: comment_data_sequential.c comment_data.c date_utils.c utils.c test.c | $(TESTS_DIR)
ifneq ($(UNAME_S),Darwin)
	$(CC) $(CFLAGS) $? -o $(TESTS_DIR)/$@ -lcheck -lm -lpthread -lrt -lsubunit -fprofile-arcs -ftest-coverage
endif
test_parallel.out: comment_data_parallel.c comment_data.c date_utils.c utils.c test.c | $(TESTS_DIR)
ifneq ($(UNAME_S),Darwin)
	$(CC) $(CFLAGS) $? -o $(TESTS_DIR)/$@ -lcheck -lm -lpthread -lrt -lsubunit -fprofile-arcs -ftest-coverage
endif
test: test_sequential.out test_parallel.out count_sequential.out count_parallel.out randgen_data.out
	python3 -m cpplint --filter=-readability/casting $(SRC_DIR)/*.c $(SRC_DIR)/include/*.h
	cppcheck --error-exitcode=1 $(SRC_DIR)/*.c $(SRC_DIR)/include/*.h
# valgrind and check does not work under macOS
ifneq ($(UNAME_S),Darwin)
	$(TESTS_DIR)/test_sequential.out
	gcov utils.c date_utils.c comment_data.c comment_data_sequential.c
	$(TESTS_DIR)/test_parallel.out
	gcov utils.c date_utils.c comment_data.c comment_data_parallel.c
	valgrind --leak-check=yes --error-exitcode=1 $(BIN_DIR)/randgen_data.out $(BIN_DIR)/test_data.txt 10000
	valgrind --leak-check=yes --error-exitcode=1 $(BIN_DIR)/count_sequential.out $(BIN_DIR)/test_data.txt
	valgrind --leak-check=yes --error-exitcode=1 env LD_LIBRARY_PATH=$(BIN_DIR) $(BIN_DIR)/count_parallel.out $(BIN_DIR)/test_data.txt
else
	cd $(SRC_DIR) && ./randgen.out test_data.txt 100000
	cd $(SRC_DIR) && ./main_sequential.out
	cd $(SRC_DIR) && ./main_parallel.out
endif

clean:
	rm -f *.gcda *.gcno *.gcov
	cd $(BUILD_DIR) && rm -f *.o *.pio *.a
	cd $(TESTS_DIR) && rm -f *.out
	cd $(BIN_DIR) && rm -f *.out *.so

$(BUILD_DIR):
	mkdir $(BUILD_DIR)
	mkdir $(BIN_DIR)
$(TESTS_DIR): $(BUILD_DIR)
	mkdir $(TESTS_DIR)
