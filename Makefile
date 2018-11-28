CFLAGS=-std=c89 -Wall -Wextra -pedantic-errors -Wmissing-prototypes -Wstrict-prototypes -Werror -O3 -g

all: tests example

tests:
	gcc $(CFLAGS) test/test.c pbg.c -o test/tests

example:
	gcc $(CFLAGS) test/example.c pbg.c -o test/example

clean:
	rm -rf test/tests test/tests.exe test/example test/example.exe
