tests:
	gcc test/tests.c pbg.c -o test/tests -g

clean:
	rm -rf test/tests.exe test/tests
