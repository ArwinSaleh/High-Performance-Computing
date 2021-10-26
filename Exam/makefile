.PHONY : all
all : cell_distances_fragmented

cell_distances_fragmented : cell_distances_fragmented.c
	echo "\nBuilding cell_distances_fragmented\n"
	gcc -o cell_distances_fragmented cell_distances_fragmented.c -fopenmp -O2 -lm
