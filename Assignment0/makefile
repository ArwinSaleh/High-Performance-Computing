.PHONY : all
all : Assignment0 printargs NaiveBenchmarking

Assignment0 : Assignment0.c
	echo "\nBuilding Assignment0\n"
	gcc -o Assignment0 Assignment0.c

printargs : printargs.c
	echo "\nBuilding printargs\n"
	gcc -o printargs printargs.c

NaiveBenchmarking : NaiveBenchmarking.c
        echo "\nBuilding NaiveBenchmarking\n"
        gcc -o NaiveBenchmarking NaiveBenchmarking.c -O0 -O1 -O2 -O3 -Os -Og
