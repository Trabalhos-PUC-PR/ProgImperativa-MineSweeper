#include <stdlib.h>
#include <stdio.h>
#include "field.h"

int fieldWidth = 0;
int fieldHeight = 0;
int **field;
/**
 * @brief the separator that will be between squares on the game
 */
char separator = '|';

/**
 * @brief allocates a field with given height and width.
 * If any of the given measures are less than zero, the field will not be allocated
 */
void allocateField(int height, int width) {
	if (height <= 1 || width <= 1) {
		height = 2;
		width = 2;
	}
	field = (int**) malloc(width * sizeof(int*));
	for (int i = 0; i < width; i++) {
		field[i] = malloc(sizeof(int) * height);
	}
	fieldWidth = width;
	fieldHeight = height;
}

/**
 * @brief prints entire field on terminal
 */
void printField() {
	for (int w = 0; w < fieldWidth; w++) {
		printf("[");
		for (int h = 0; h < fieldHeight; h++) {
			printf("%d", field[w][h]);
			if (h != fieldHeight - 1) {
				printf("%c", separator);
			} else {
				printf("]\n");
			}
		}
	}
}
