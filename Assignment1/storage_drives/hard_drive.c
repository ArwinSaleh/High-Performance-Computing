#include <stdio.h>
#include <stdlib.h>

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

    writeToFile("file.dat");
    readAndCheckFile("file.dat");

}