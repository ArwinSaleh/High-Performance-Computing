#include <stdio.h>
#include <stdlib.h>

void stackAllocation(){

    /*
    Allocating is performed on the stack, 
    meaning when we increase the size of
    the array, the compiler tries to write
    to a read-only segment which is reserved
    for other processes.
    */

    const int size = 10000000;  // Segmentation fault at 10^7.
    int as[size];   // Stack 
    for ( size_t ix = 0; ix < size; ++ix ){
        as[ix] = 0;
    }

    printf("%d\n", as[0]);
}

void heapAllocation(){

    /*
    With heap allocation we avoid
    segmentation fault, but this
    approach is slower.
    */

    const int size = 10000000;  // No segmentation fault at 10^7
    int * as = (int*) malloc(sizeof(int) * size);   // Heap
    for ( size_t ix = 0; ix < size; ++ix )
    as[ix] = 0;

    printf("%d\n", as[0]);

    free(as);
}

void memoryFragmentation(){
    int size = 10;
    
    int ** as = (int**) malloc(sizeof(int*) * size);
    for ( size_t ix = 0; ix < size; ++ix ){
        as[ix] = (int*) malloc(sizeof(int) * size);
    }

    for ( size_t ix = 0; ix < size; ++ix ){
        for ( size_t jx = 0; jx < size; ++jx ){
            as[ix][jx] = 0;
        }
    }
    

    printf("%d\n", as[0][0]);

    for ( size_t ix = 0; ix < size; ++ix ){
        free(as[ix]);
    }   
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

void writeToFile(char *fileName){
    const int size = 10;

    FILE *data_file = fopen(fileName, "w");
    for ( size_t ix = 0; ix < size; ++ix ){
        for ( size_t jx = 0; jx < size; ++jx ){
            fprintf(data_file, "%ld\t", ix*jx);
        }
        fprintf(data_file, "\n\n");
    }
    fclose(data_file);
    printf("\nMatrix data has been stored in %s\n", fileName);
}

void readAndCheckFile(char *fileName)
{
    const int size = 10;
    int isCorrect = 1;

    FILE *data_file = fopen(fileName, "r");
    for ( size_t ix = 0; ix < size; ++ix )
    {
        for ( size_t jx = 0; jx < size; ++jx )
        {
            size_t currentElement;
            fscanf(data_file, "%ld", &currentElement);
            if (currentElement != ix * jx)
            {
                isCorrect = 0;
                break;
            }
            if (!isCorrect)
            {
                break;
            }
        }
    }
    fclose(data_file);
    
    if (isCorrect){
        printf("\nEach element in the %s matrix is equal to ix*jx\n", fileName);
    }
    else if (!isCorrect)
    {
        printf("\nERROR! Each element in the %s matrix is NOT equal to ix*jx\n", fileName);
    }
}

int main(){

    //stackAllocation();
    //heapAllocation();
    //memoryFragmentation();
    //noMemoryFragmentation();
    writeToFile("file.dat");
    readAndCheckFile("file.dat");
}