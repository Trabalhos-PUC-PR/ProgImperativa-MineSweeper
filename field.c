#include <stdlib.h>
#include <stdio.h>
#include "field.h"
#include "boolean.h"
#include "square.h"
#include "randomGen.h"

int fieldWidth = 0;
int fieldHeight = 0;
int totalBombs = 0;
int totalSquares = 0;
int totalSafeSquares = 0;
int totalFlaggedSquares = 0;
int totalSquaresRevealed = 0;
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
	totalSquares = width * height;
}

enum boolean fieldRevealAt(int height, int width) {
	if (!fieldIsValidPos(height, width)) {
		return false;
	}
	if (field[height][width].isFlagged) {
		return false;
	}
	if (field[height][width].isRevealed) {
		return false;
	}
	field[height][width].isRevealed = true;
	totalSquaresRevealed++;
	if (field[height][width].number == 0)
		fieldRevealEmptyField(height, width);
	return true;
}

void fieldRevealEmptyField(int height, int width) {
	// four cardinal directions
	fieldRevealAt(height, width - 1);
	fieldRevealAt(height - 1, width);
	fieldRevealAt(height, width + 1);
	fieldRevealAt(height + 1, width);
	fieldRevealAt(height - 1, width - 1);
	fieldRevealAt(height - 1, width + 1);
	fieldRevealAt(height + 1, width - 1);
	fieldRevealAt(height + 1, width + 1);
}

enum boolean fieldSetFlagAt(int height, int width){
	if (!fieldIsValidPos(height, width)) {
		return false;
	}
	if (field[height][width].isRevealed) {
		return false;
	}
	if(field[height][width].isFlagged){
		totalFlaggedSquares--;
	}else{
		totalFlaggedSquares++;
	}
	field[height][width].isFlagged = !field[height][width].isFlagged;
	return true;
}

enum boolean fieldIsValidPos(int height, int width) {
	if (height < 0 || height >= fieldWidth) {
		return false;
	}

	if (width < 0 || width >= fieldHeight) {
		return false;
	}

	return true;
}

void fieldRevealAll() {
	for (int h = 0; h < fieldHeight; h++) {
		for (int w = 0; w < fieldWidth; w++) {
			field[h][w].isRevealed = true;
		}
	}
}

void fieldSetRandomBombs(int total) {
	randomSetupSeed();
	for (int i = 0; i < total; i++) {
		enum boolean setted = fieldSetBomb(randomNum(fieldHeight - 1),
				randomNum(fieldWidth - 1));
		while (setted == false) {
			setted = fieldSetBomb(randomNum(fieldHeight - 1),
					randomNum(fieldWidth - 1));
		}
	}
	totalBombs = total;
	totalSafeSquares = totalSquares - totalBombs;
}

/**
 * Sets a bomb in given coordinate, returns false if its unable to set bomb
 */
enum boolean fieldSetBomb(int height, int width) {
	if (height < 0 || height >= fieldWidth) {
		return false;
	}
	if (width < 0 || width >= fieldHeight) {
		return false;
	}
	if (field[height][width].isBomb) {
		return false;
	}
	if (field[height][width].isFlagged) {
		return false;
	}
	field[height][width].isBomb = true;
	field[height][width].number = 9;

	if (height - 1 >= 0) {
		field[height - 1][width].number += 1;
		field[height - 1][width + 1].number += 1;
		field[height - 1][width - 1].number += 1;
	}
	if (height + 1 < fieldHeight) {
		field[height + 1][width].number += 1;
		field[height + 1][width + 1].number += 1;
		field[height + 1][width - 1].number += 1;
	}
	if (width + 1 < fieldWidth)
		field[height][width + 1].number += 1;
	if (width - 1 >= 0)
		field[height][width - 1].number += 1;

	totalBombs++;
	return true;
}

/**
 * @brief prints entire field on terminal
 */
void fieldPrint() {
	for (int w = 0; w < fieldWidth; w++) {
		printf("[");
		for (int h = 0; h < fieldHeight; h++) {
			if (field[w][h].isFlagged) {
				printf("F");
			} else {
				if (field[w][h].isRevealed) {
					if (field[w][h].number >= 9) {
						printf("x");
					} else {
						if (field[w][h].number == 0) {
							printf(" ");
						} else {
							printf("%d", field[w][h].number);
						}
					}
				} else {
					printf("-");
				}
			}

			if (h != fieldHeight - 1) {
				printf("%c", separator);
			} else {
				printf("]\n");
			}
		}
	}
}
