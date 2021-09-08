.PHONY : all
all : Assignment1 NaiveBenchmarking

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