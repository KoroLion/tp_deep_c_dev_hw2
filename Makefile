test:
	cd src && \
	python3 -m cpplint *.c && \
	cppcheck --error-exitcode=1 *.c include/*.h && \
	gcc main.c -o main.out -Wall -Werror -Wpedantic && \
	valgrind --leak-check=yes --error-exitcode=1 ./main.out
run:
	cd src && ./main.out
