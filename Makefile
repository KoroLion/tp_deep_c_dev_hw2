VPATH:=src

CC:=gcc
CFLAGS:=-Wall -Werror -Wpedantic
SRC_DIR:=src
BUILD_DIR:=src
UNAME_S:=$(shell uname -s)

all: main.out randgen.out

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $(SRC_DIR)/$@
libcomment_data.a: comment_data.o date_utils.o
	cd $(SRC_DIR) && ar -rc libcomment_data.a comment_data.o date_utils.o
librandom_data_gen.a: random_data_gen.o date_utils.o
	cd $(SRC_DIR) && ar -rc librandom_data_gen.a random_data_gen.o date_utils.o
randgen.out: randgen.c librandom_data_gen.a
	cd $(SRC_DIR) && $(CC) -o randgen.out randgen.c -L'.' -l'random_data_gen' $(CFLAGS)
main.out: main.c libcomment_data.a
	cd $(SRC_DIR) && $(CC) -o main.out main.c -L'.' -l'comment_data' -lpthread $(CFLAGS)

generate: randgen.out
	cd $(SRC_DIR) && ./randgen.out test_data.txt 10000000
run: main.out
	cd $(SRC_DIR) && ./main.out
test.out: test.c
ifneq ($(UNAME_S),Darwin)
	cd $(SRC_DIR) && gcc test.c comment_data.c date_utils.c -o test.out -lcheck -lm -lpthread -lrt -lsubunit -fprofile-arcs -ftest-coverage $(CFLAGS)
endif
test: test.out main.out randgen.out
	python3 -m cpplint --filter=-readability/casting $(SRC_DIR)/*.c $(SRC_DIR)/include/*.h
	cppcheck --error-exitcode=1 $(SRC_DIR)/*.c $(SRC_DIR)/include/*.h
# valgrind and check does not work under macOS
ifneq ($(UNAME_S),Darwin)
	cd $(SRC_DIR) && ./test.out
	cd $(SRC_DIR) && valgrind --leak-check=yes --error-exitcode=1 ./randgen.out test_data.txt 10000
	cd $(SRC_DIR) && valgrind --leak-check=yes --error-exitcode=1 ./main.out
	cd $(SRC_DIR) && gcov date_utils.c comment_data.c random_data_gen.c
else
	cd $(SRC_DIR) && ./randgen.out test_data.txt 100000
	cd $(SRC_DIR) && ./main.out
endif
clean:
	cd $(SRC_DIR) && rm -f *.o *.gcno *.gcda *.out *.gcov *.exe *.a
