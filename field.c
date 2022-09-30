#include <stdlib.h>
#include <stdio.h>
#include "field.h"
#include "boolean.h"
#include "square.h"

int fieldWidth = 0;
int fieldHeight = 0;
struct Square **field;
/**
 * @brief the separator that will be between squares on the game
 */
char separator = '|';

/**
 * @brief allocates a field with given height and width.
 * If any of the given measures are less than two, the field will be set as 2x2
 */
void allocateField(int width, int height) {
	if (height <= 1 || width <= 1) {
		height = 2;
		width = 2;
	}
	field = (struct Square**) malloc(width * sizeof(struct Square*));
	for (int w = 0; w < width; w++) {
		field[w] = malloc(sizeof(struct Square) * height);
	}
	fieldWidth = width;
	fieldHeight = height;
}

enum boolean fieldSetBomb(int height, int width) {
	if (height < 0 || height >= fieldWidth) {
		return false;
	}
	if (width < 0 || width >= fieldHeight) {
		return false;
	}
	field[height][width].isBomb = true;
	return true;
}

/**
 * @brief prints entire field on terminal
 */
void printField() {
	for (int w = 0; w < fieldWidth; w++) {
		printf("[");
		for (int h = 0; h < fieldHeight; h++) {
			printf("%d", field[w][h].isBomb);
			if (h != fieldHeight - 1) {
				printf("%c", separator);
			} else {
				printf("]\n");
			}
		}
	}
}
