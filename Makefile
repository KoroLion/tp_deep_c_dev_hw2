VPATH:=src

CC:=gcc
CFLAGS:=-Wall -Werror -Wpedantic
SRC_DIR:=src
BUILD_DIR:=src
UNAME_S:=$(shell uname -s)

all: main_sequential.out main_parallel.out randgen.out

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $(SRC_DIR)/$@
libcomment_data_parallel.so: comment_data_parallel.c comment_data.c date_utils.c
	cd $(SRC_DIR) && gcc -fPIC -c comment_data.c comment_data_parallel.c date_utils.c
	cd $(SRC_DIR) && gcc -shared -o libcomment_data_parallel.so comment_data.o date_utils.o comment_data_parallel.o -lpthread

libcomment_data.a: comment_data_sequential.o comment_data.o date_utils.o
	cd $(SRC_DIR) && ar -rc libcomment_data.a comment_data_sequential.o comment_data.o date_utils.o
librandom_data_gen.a: random_data_gen.o date_utils.o comment_data_sequential.o
	cd $(SRC_DIR) && ar -rc librandom_data_gen.a random_data_gen.o date_utils.o
randgen.out: randgen.c librandom_data_gen.a
	cd $(SRC_DIR) && $(CC) -o randgen.out randgen.c -L'.' -l'random_data_gen' $(CFLAGS)

# dynamic library for paralell alg
main_parallel.out: main.c libcomment_data_parallel.so
	cd $(SRC_DIR) && $(CC) -o main_parallel.out main.c -L'.' -l'comment_data_parallel' -lpthread $(CFLAGS)
# static library for sequential alg
main_sequential.out: main.c libcomment_data.a
	cd $(SRC_DIR) && $(CC) -o main_sequential.out main.c -L'.' -l'comment_data' -lpthread $(CFLAGS)

generate: randgen.out
	cd $(SRC_DIR) && ./randgen.out test_data.txt 10000000
run_seq: main_sequential.out
	cd $(SRC_DIR) && ./main_sequential.out
run_par: main_parallel.out
	cd $(SRC_DIR) && LD_LIBRARY_PATH=. ./main_parallel.out

test_sequential.out: test.c
ifneq ($(UNAME_S),Darwin)
	cd $(SRC_DIR) && gcc test.c comment_data.c comment_data_parallel.c date_utils.c -o test_sequential.out -lcheck -lm -lpthread -lrt -lsubunit -fprofile-arcs -ftest-coverage $(CFLAGS)
endif
test_parallel.out: test.c
ifneq ($(UNAME_S),Darwin)
	cd $(SRC_DIR) && gcc test.c comment_data.c comment_data_sequential.c date_utils.c -o test_parallel.out -lcheck -lm -lpthread -lrt -lsubunit -fprofile-arcs -ftest-coverage $(CFLAGS)
endif
test: test_sequential.out test_parallel.out main_sequential.out main_parallel.out randgen.out
	python3 -m cpplint --filter=-readability/casting $(SRC_DIR)/*.c $(SRC_DIR)/include/*.h
	cppcheck --error-exitcode=1 $(SRC_DIR)/*.c $(SRC_DIR)/include/*.h
# valgrind and check does not work under macOS
ifneq ($(UNAME_S),Darwin)
	cd $(SRC_DIR) && ./test_sequential.out
	cd $(SRC_DIR) && ./test_parallel.out
	cd $(SRC_DIR) && valgrind --leak-check=yes --error-exitcode=1 ./randgen.out test_data.txt 10000
	cd $(SRC_DIR) && valgrind --leak-check=yes --error-exitcode=1 ./main_sequential.out
	cd $(SRC_DIR) && valgrind --leak-check=yes --error-exitcode=1 env LD_LIBRARY_PATH=. ./main_parallel.out
	cd $(SRC_DIR) && gcov date_utils.c comment_data.c comment_data_sequential.c comment_data_parallel.c
else
	cd $(SRC_DIR) && ./randgen.out test_data.txt 100000
	cd $(SRC_DIR) && ./main_sequential.out
	cd $(SRC_DIR) && ./main_parallel.out
endif
clean:
	cd $(SRC_DIR) && rm -f *.o *.gcno *.gcda *.out *.gcov *.exe *.a *.so
