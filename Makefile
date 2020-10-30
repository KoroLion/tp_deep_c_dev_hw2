# Copyright 2020 KoroLion (github.com/KoroLion)

# $@ - name of target
# $< - name of first req
# $^ - all req with space between
# pic_o is a directory for Position Independent code object files

SHELL:=/bin/bash

SRC_DIR:=src
BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin
TESTS_DIR := $(BUILD_DIR)/tests
PICOBJ_DIR := $(BUILD_DIR)/pic_o

BINARIES = $(addprefix $(BIN_DIR)/,randgen_data.out count_sequential.out count_parallel.out)

CC:=gcc
CFLAGS:=-Wall -Werror -Wpedantic
UNAME_S:=$(shell uname -s)

all: $(BINARIES) | $(BUILD_DIR)
run: $(BINARIES) | $(BUILD_DIR)
	@echo --- Runnning...
	@$(BIN_DIR)/randgen_data.out test_data.txt 10000
	@echo Sequential:
	@$(BIN_DIR)/count_sequential.out test_data.txt
	@echo Parallel:
	@LD_LIBRARY_PATH=$(BIN_DIR) $(BIN_DIR)/count_parallel.out test_data.txt

# building objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -c -o $@
$(PICOBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -fPIC -c -o $@
$(TESTS_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
ifneq ($(UNAME_S),Darwin)
	$(CC) $(CFLAGS) $< -c -o $@ -fprofile-arcs -ftest-coverage -lgcov
else
	@echo --- Warning: tests are not supported under macOS
endif

# building libs
$(BUILD_DIR)/librandom_data_gen.a: $(addprefix $(BUILD_DIR)/,date_utils.o comment_data.o random_data_gen.o)
	ar -rc $@ $^
$(BUILD_DIR)/libcomment_data_sequential.a: $(addprefix $(BUILD_DIR)/,comment_data_sequential.o utils.o date_utils.o comment_data.o)
	ar -rc $@ $^
$(BIN_DIR)/libcomment_data_parallel.so: $(addprefix $(PICOBJ_DIR)/,comment_data_parallel.o utils.o date_utils.o comment_data.o)
	$(CC) $(CFLAGS) -shared -o $@ $^ -lpthread

# build bins
# random comments data file generator
$(BIN_DIR)/randgen_data.out: $(SRC_DIR)/randgen.c $(BUILD_DIR)/librandom_data_gen.a
	$(CC) $(CFLAGS) -o $@ $< -L$(BUILD_DIR) -l'random_data_gen'
# with static library for sequential alg
$(BIN_DIR)/count_sequential.out: $(SRC_DIR)/main.c $(BUILD_DIR)/libcomment_data_sequential.a
	$(CC) $(CFLAGS) -o $@ $< -L$(BUILD_DIR) -l'comment_data_sequential'
# with dynamic library for paralell alg
$(BIN_DIR)/count_parallel.out: $(SRC_DIR)/main.c $(BIN_DIR)/libcomment_data_parallel.so
	$(CC) $(CFLAGS) -o $@ $< -L$(BIN_DIR) -l'comment_data_parallel' -lpthread

$(TESTS_DIR)/test_sequential.out: $(addprefix $(TESTS_DIR)/,comment_data_sequential.o comment_data.o date_utils.o utils.o test.o)
ifneq ($(UNAME_S),Darwin)
	$(CC) $(CFLAGS) $^ -o $@ -lcheck -lm -lpthread -lrt -lsubunit -lgcov
endif
$(TESTS_DIR)/test_parallel.out: $(addprefix $(TESTS_DIR)/,comment_data_parallel.o comment_data.o date_utils.o utils.o test.o)
ifneq ($(UNAME_S),Darwin)
	$(CC) $(CFLAGS) $^ -o $@ -lcheck -lm -lpthread -lrt -lsubunit -lgcov
endif
test: $(addprefix $(TESTS_DIR)/,test_sequential.out test_parallel.out) $(BINARIES) | $(BUILD_DIR)
	python3 -m cpplint --filter=-readability/casting $(SRC_DIR)/*.c $(SRC_DIR)/include/*.h
	cppcheck --error-exitcode=1 $(SRC_DIR)/*.c $(SRC_DIR)/include/*.h
# valgrind and check does not work under macOS
ifneq ($(UNAME_S),Darwin)
	valgrind --leak-check=yes --error-exitcode=1 $(BIN_DIR)/randgen_data.out $(BIN_DIR)/test_data.txt 10000
	valgrind --leak-check=yes --error-exitcode=1 $(BIN_DIR)/count_sequential.out $(BIN_DIR)/test_data.txt
	valgrind --leak-check=yes --error-exitcode=1 env LD_LIBRARY_PATH=$(BIN_DIR) $(BIN_DIR)/count_parallel.out $(BIN_DIR)/test_data.txt
	$(TESTS_DIR)/test_sequential.out
	gcov $(addprefix $(TESTS_DIR)/,comment_data_sequential.o utils.to date_utils.o comment_data.o) --object-directory $(TESTS_DIR)
	$(TESTS_DIR)/test_parallel.out
	gcov $(addprefix $(TESTS_DIR)/,comment_data_parallel.o utils.to date_utils.o comment_data.o) --object-directory $(TESTS_DIR)
	rm *.gcov
else
	@echo --- Warning: valgrind does not work under macOS
	$(BIN_DIR)/randgen_data.out test_data.txt 100000
	$(BIN_DIR)/count_sequential.out test_data.txt
	$(BIN_DIR)/count_parallel.out test_data.txt
endif

stress_test: $(BINARIES)
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

clean:
	rm -f $(BIN_DIR)/*.out $(BIN_DIR)/*.so $(BUILD_DIR)/*.o $(BUILD_DIR)/*.a $(PICOBJ_DIR)/*.o

$(BUILD_DIR) $(PICOBJ_DIR) $(TESTS_DIR) $(BIN_DIR):
	mkdir -p $(BIN_DIR)
	mkdir -p $(PICOBJ_DIR)
	mkdir -p $(TESTS_DIR)
