.PHONY : all
all : same_file different_file inlined_manually

same_file : same_file.c helper_functions.h
	echo "\nBuilding same_file\n"
	gcc -o same_file same_file.c

different_file : different_file.c helper_functions.h
	echo "\nBuilding different_file\n"
	gcc -o different_file different_file.c

inlined_manually : inlined_manually.c helper_functions.h
	echo "\nBuilding different_file\n"
	gcc -o inlined_manually inlined_manually.c
