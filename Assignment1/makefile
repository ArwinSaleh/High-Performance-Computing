.PHONY : all
all : Assignment1 NaiveBenchmarking test_valgrind test_gdb same_file

Assignment1 : Assignment1.c
	echo "\nBuilding Assignment1\n"
	gcc -o Assignment1 Assignment1.c

NaiveBenchmarking : NaiveBenchmarking.c
	echo "\nBuilding NaiveBenchmarking\n"
	gcc -o NaiveBenchmarking NaiveBenchmarking.c -O0 -O1 -O2 -Os -Og
	gcc -S -O0 NaiveBenchmarking.c -o NaiveBenchmarking_O0.s
	gcc -S -O1 NaiveBenchmarking.c -o NaiveBenchmarking_O1.s
	gcc -S -O2 NaiveBenchmarking.c -o NaiveBenchmarking_O2.s
	gcc -S -Os NaiveBenchmarking.c -o NaiveBenchmarking_Os.s
	gcc -S -Og NaiveBenchmarking.c -o NaiveBenchmarking_Og.s
	gcc -S -O0 -O1 -O2 -Os -Og NaiveBenchmarking.c -o NaiveBenchmarking_ALL.s

test_valgrind : test_valgrind.c
	echo "\nBuilding test_valgrind\n"
	gcc -o test_valgrind test_valgrind.c

test_gdb : test_gdb.c
	echo "\nBuilding test_gdb\n"
	gcc -o test_gdb test_gdb.c

same_file : same_file.c
	echo "\nBuilding same_file\n"
	gcc -o same_file same_file.c
