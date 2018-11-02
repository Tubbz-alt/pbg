tests:
	gcc test/tests.c pbg.c -o test/tests

clean:
	rm -rf test/tests.exe test/tests
