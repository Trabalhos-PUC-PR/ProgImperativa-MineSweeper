#include "field.h"

int fieldWidth = 0;
int fieldHeight = 0;
int **field;

void allocateField(int height, int width){
    field = (int**)malloc(width*sizeof(int*));
    for(int i = 0; i < width; i++){
        field[i] = malloc(sizeof(int)*height);
    }
    fieldWidth = width;
    fieldHeight = height;
}

void printField(){
    for (int w = 0; w < fieldWidth; w++){
        printf("[");
        for (int h = 0; h < fieldHeight; h++){
            printf("%d",field[w][h]);
            if(h != fieldHeight-1){
                printf(".");
            }else{
                printf("]\n");
            }
        }
    }
}