compile_command = gcc main.c utils.c comment_data.c -o main.out
UNAME_S := $(shell uname -s)

target:
	cd src && $(compile_command)
run:
	cd src && ./main.out
test:
	cd src && \
	python3 -m cpplint *.c include/*.h && \
	cppcheck --error-exitcode=1 *.c include/*.h && \
	$(compile_command) -Wall -Werror -Wpedantic
# valgrind does not work under macOS
ifneq ($(UNAME_S),Darwin)
	cd src && valgrind --leak-check=yes --error-exitcode=1 ./main.out
endif
