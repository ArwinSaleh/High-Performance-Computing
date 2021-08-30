#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void stackAllocation(){

    /*
    Allocating is performed on the stack, 
    meaning when we increase the size of
    the array, the compiler tries to write
    to a read-only segment which is reserved
    for other processes.
    */

    int size = pow(10, 7);  // Segmentation fault at 10^7.
    //int size = pow(10, 6);
    int as[size];
    for ( size_t ix = 0; ix < size; ++ix )
    as[ix] = 0;

    printf("%d\n", as[0]);
}

void heapAllocation(){

    /*
    With heap allocation we avoid
    segmentation fault, but this
    approach is slower.
    */

    int size = pow(10, 7);  // No segmentation fault at 10^7
    int * as = (int*) malloc(sizeof(int) * size);
    for ( size_t ix = 0; ix < size; ++ix )
    as[ix] = 0;

    printf("%d\n", as[0]);

    free(as);
}

void memoryFragmentation(){
    int size = 10;
    
    int ** as = (int**) malloc(sizeof(int*) * size);
    for ( size_t ix = 0; ix < size; ++ix )
    as[ix] = (int*) malloc(sizeof(int) * size);

    for ( size_t ix = 0; ix < size; ++ix )
    for ( size_t jx = 0; jx < size; ++jx )
        as[ix][jx] = 0;

    printf("%d\n", as[0][0]);

    for ( size_t ix = 0; ix < size; ++ix )
        free(as[ix]);
    free(as);
}

void noMemoryFragmentation(){
    int size = 10;

    int * asentries = (int*) malloc(sizeof(int) * size*size);
    int ** as = (int**) malloc(sizeof(int*) * size);
    for ( size_t ix = 0, jx = 0; ix < size; ++ix, jx+=size )
    as[ix] = asentries + jx;

    for ( size_t ix = 0; ix < size; ++ix )
    for ( size_t jx = 0; jx < size; ++jx )
        as[ix][jx] = 0;

    printf("%d\n", as[0][0]);

    free(as);
    free(asentries);
}

int main(int argc, char *argv[]){

    //stackAllocation();
    //heapAllocation();
    //memoryFragmentation();
    noMemoryFragmentation();
}