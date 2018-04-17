CC=clang
CFLAGS=-Wall -Werror -Winline -std=c11 -g

TESTS = string_util_test hash_test list_test map_test

all: $(TESTS)
	@for test in $(TESTS); do \
		echo Running $$test...; \
		./$$test || exit 1; \
		echo Running valgrind on $$test...; \
		valgrind --leak-check=yes --error-exitcode=1 ./$$test \
			|| exit 1; \
	done

string_util.o: string_util.h errors.h
hash.o: hash.c hash.h
list.o: list.c list.h errors.h
map.o: map.c map.h hash.h errors.h

string_util_test: string_util_test.c string_util.o
list_test: list_test.c list.o
hash_test: hash_test.c hash.o
map_test: map_test.c map.o hash.o list.o string_util.o

clean:
	rm -rf *.o $(TESTS)
