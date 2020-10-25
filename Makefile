VPATH:=src

CC:=gcc
CFLAGS:=-Wall -Werror -Wpedantic
SRC_DIR:=src
BUILD_DIR:=src
UNAME_S:=$(shell uname -s)

all: main.out randgen.out

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $(SRC_DIR)/$@
comment_data.a: comment_data.o date_utils.o
	cd $(SRC_DIR) && ar -rc comment_data.a comment_data.o date_utils.o
random_data_gen.a: random_data_gen.o date_utils.o
	cd $(SRC_DIR) && ar -rc random_data_gen.a random_data_gen.o date_utils.o
randgen.out: random_data_gen.a
	cd $(SRC_DIR) && $(CC) -o randgen.out randgen.c random_data_gen.a $(CFLAGS)
main.out: comment_data.a
	cd $(SRC_DIR) && $(CC) -o main.out main.c comment_data.a $(CFLAGS)

run: main.out randgen.out
	cd $(SRC_DIR) && ./randgen.out test_data.txt 10000
	cd $(SRC_DIR) && ./main.out
test.out: test.c
	cd $(SRC_DIR) && gcc test.c comment_data.c date_utils.c -o test.out -lcheck -lm -lpthread -lrt -lsubunit -fprofile-arcs -ftest-coverage $(CFLAGS)
test: test.out main.out randgen.out
	python3 -m cpplint --filter=-readability/casting $(SRC_DIR)/*.c $(SRC_DIR)/include/*.h
	cppcheck --error-exitcode=1 $(SRC_DIR)/*.c $(SRC_DIR)/include/*.h
	cd $(SRC_DIR) && ./test.out
# valgrind does not work under macOS
# check needs futher setup
ifneq ($(UNAME_S),Darwin)
	cd $(SRC_DIR) && valgrind --leak-check=yes --error-exitcode=1 ./randgen.out test_data.txt 10000
	cd $(SRC_DIR) && valgrind --leak-check=yes --error-exitcode=1 ./main.out
	cd $(SRC_DIR) && gcov date_utils.c comment_data.c random_data_gen.c
endif
clean:
	cd $(SRC_DIR) && rm -f *.o *.gcno *.gcda *.out *.gcov *.exe *.a
