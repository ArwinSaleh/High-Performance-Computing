.PHONY : all
all : test_valgrind test_gdb

test_valgrind : test_valgrind.c
	echo "\nBuilding test_valgrind\n"
	gcc -o test_valgrind test_valgrind.c

test_gdb : test_gdb.c
	echo "\nBuilding test_gdb\n"
	gcc -o test_gdb test_gdb.c