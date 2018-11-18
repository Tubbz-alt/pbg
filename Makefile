CFLAGS=-Wall -Wextra -pedantic-errors -Wmissing-prototypes -Wstrict-prototypes -Werror -O3 -g

all:
	gcc $(CFLAGS) test/pbg_test.c pbg.c -o test/tests

clean:
	rm -rf test/tests test/tests.exe
