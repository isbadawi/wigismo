chash.o: chash.c chash.h
	gcc -c chash.c

test: chash.o minunit.h chash_tests.c
	@gcc -c chash_tests.c
	@nm chash_tests.o | grep test_ | cut -d " " -f 3 | \
	 python generate_test_suite.py > real_tests.c
	@gcc real_tests.c chash.o -o tests
	@./tests
	@rm chash_tests.o real_tests.c tests

test-debug: chash.o minunit.h chash_tests.c
	@gcc -c -g chash_tests.c
	@nm chash_tests.o | grep test_ | cut -d " " -f 3 | \
	 python generate_test_suite.py > real_tests.c
	@gcc -g real_tests.c chash.o -o tests
	@gdb tests
	@rm chash_tests.o real_tests.c tests
