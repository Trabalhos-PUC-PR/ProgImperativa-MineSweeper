#include "field.h"

#define fieldWidth 3
#define fieldHeight 3

int field[fieldWidth][fieldHeight];

void printField(){
    for (int w = 0; w < fieldWidth; w++){
        printf("[");
        for (int h = 0; h < fieldHeight; h++){
            printf("%d",field[w][h]);
            if(h == fieldHeight-1){
                printf("]\n");
            }
        }
    }
}