compile_main_command = gcc main.c utils.c comment_data.c -o main.out
compile_randgen_command = gcc randgen.c utils.c comment_data.c -o randgen.out
compile_test_command = gcc test.c utils.c comment_data.c -o test.out -lcheck -lm -lpthread -lrt -lsubunit -fprofile-arcs -ftest-coverage
UNAME_S := $(shell uname -s)

target:
	cd src && \
	$(compile_main_command) && \
	$(compile_randgen_command)
run:
	cd src && \
	./randgen.out test_data.txt 1000 && \
	./main.out
test:
	cd src && \
	python3 -m cpplint *.c include/*.h && \
	cppcheck --error-exitcode=1 *.c include/*.h && \
	$(compile_main_command) -Wall -Werror -Wpedantic && \
	$(compile_randgen_command) -Wall -Werror -Wpedantic
# valgrind does not work under macOS
# check needs futher setup
ifneq ($(UNAME_S),Darwin)
	cd src && \
	$(compile_test_command) && \
	./test.out && \
	valgrind --leak-check=yes --error-exitcode=1 ./randgen.out test_data.txt && \
	valgrind --leak-check=yes --error-exitcode=1 ./main.out && \
	gcov utils.c comment_data.c
endif
