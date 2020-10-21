compile_main_command = gcc main.c utils.c comment_data.c -o main.out
compile_randgen_command = gcc randgen.c utils.c comment_data.c -o randgen.out
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
ifneq ($(UNAME_S),Darwin)
	cd src && \
	valgrind --leak-check=yes --error-exitcode=1 ./randgen.out test_data.txt && \
	valgrind --leak-check=yes --error-exitcode=1 ./main.out
endif
