tests:
	gcc test/pbg_test.c pbg.c -o test/tests -g

clean:
	rm -rf test/tests test/tests.exe
