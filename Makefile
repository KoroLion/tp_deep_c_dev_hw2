compile_command = cd src && gcc main.c -o main.out
target:
	$(compile_command)
run:
	cd src && ./main.out
test:
	python3 -m cpplint src/*.c
	cppcheck --error-exitcode=1 src/*.c src/include/*.h
	$(compile_command) -Wall -Werror -Wpedantic
# valgrind does not work under macOS and Windows
ifneq ($(OS),Windows_NT)
UNAME_S := $(shell uname -s)
ifneq ($(UNAME_S),Darwin)
	cd src && valgrind --leak-check=yes --error-exitcode=1 ./main.out
endif
endif
