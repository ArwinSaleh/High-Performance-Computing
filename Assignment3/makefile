.PHONY : all
all: newton

newton: newton.c
	echo "\nBuilding newton\n"
	gcc -o newton newton.c -Ofast -march=native -lpthread

clean:
	rm newton newton_attractors_x*.ppm newton_convergence_x*.ppm *.png
