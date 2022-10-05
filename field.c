#include <stdlib.h>
#include <stdio.h>
#include "field.h"
#include "boolean.h"
#include "square.h"
#include "randomGen.h"

enum boolean game = true;

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
void allocateField(int height, int width) {
	if (height <= 1 || width <= 1) {
		height = 2;
		width = 2;
	}
	field = (struct Square**) malloc(height * sizeof(struct Square*));
	for (int h = 0; h < height; h++) {
		field[h] = malloc(sizeof(struct Square) * width);
		for (int w = 0; w < width; w++) {
			field[h][w].number = 0;
			field[h][w].isFlagged = false;
			field[h][w].isRevealed = false;
			field[h][w].isBomb = false;
			field[h][w].yPos = h;
			field[h][w].xPos = w;
		}
	}
	fieldWidth = width;
	fieldHeight = height;
	totalSquares = width * height;
}

enum boolean fieldRevealAround(int height, int width) {
	int size = 8;
	struct Square **squares = fieldGetNeighborSquares(height, width, &size);

	for (int i = 0; i < size; i++) {
		if (!squares[i]->isRevealed && !squares[i]->isFlagged) {
			if (squares[i]->number == 0) {
				fieldRevealEmptyField(squares[i]->yPos, squares[i]->xPos);
			} else {
				squares[i]->isRevealed = true;

				if (squares[i]->isBomb) {
					return false;
				} else {
					totalSquaresRevealed++;
				}
			}
		}
	}
	free(squares);

	return true;
}

enum boolean fieldRevealAt(int height, int width) {
	if (!fieldIsValidPos(height, width)) {
		return true;
	}
	if (field[height][width].isFlagged) {
		return true;
	}
	if (field[height][width].isRevealed) {
		if (field[height][width].number == 0) {
			return true;
		}
		int sum = fieldGetSumOfNeighborFlagsAt(height, width);
		if (sum == field[height][width].number) {
			return fieldRevealAround(height, width);
		}
		return false;
	}
	field[height][width].isRevealed = true;
	if (field[height][width].isBomb) {
		return false;
	}
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
	// diagonals
	fieldRevealAt(height - 1, width - 1);
	fieldRevealAt(height - 1, width + 1);
	fieldRevealAt(height + 1, width - 1);
	fieldRevealAt(height + 1, width + 1);
}

enum boolean fieldSetFlagAt(int height, int width) {
	if (!fieldIsValidPos(height, width)) {
		return false;
	}
	if (field[height][width].isRevealed) {
		return false;
	}
	if (field[height][width].isFlagged) {
		totalFlaggedSquares--;
	} else {
		totalFlaggedSquares++;
	}
	field[height][width].isFlagged = !field[height][width].isFlagged;
	return true;
}

enum boolean fieldIsThisAFlag(int height, int width) {
	return field[height][width].isFlagged;
}

enum boolean fieldIsThisABomb(int height, int width) {
	return field[height][width].isBomb;
}

enum boolean fieldIsValidPos(int height, int width) {
	if (height < 0 || height >= fieldHeight) {
		return false;
	}

	if (width < 0 || width >= fieldWidth) {
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

struct Square** fieldGetNeighborSquares(int height, int width, int *size) {

	struct Square **neighbors = malloc(sizeof(struct Square) * *size);

	int offsetW = 3;
	int offsetH = 3;
	int beginW = width - 1;
	int beginH = height - 1;

	if (beginW < 0) {
		offsetW--;
		beginW = width;
	}
	if (fieldWidth <= width + 1)
		offsetW--;

	if (beginH < 0) {
		offsetH--;
		beginH = height;
	}
	if (fieldHeight <= height + 1)
		offsetH--;

//	printf("OffsetW: %d|BeginW: %d|\nOffsetH: %d|BeginW: %d|\nfw: %d|fh: %d|\n", offsetW, beginW, offsetH, beginH, fieldWidth, fieldHeight);

	int count = 0;
	for (int h = beginH; h < offsetH + beginH; h++) {
		for (int w = beginW; w < offsetW + beginW; w++) {
			if (w != width || h != height) {
				neighbors[count] = &field[h][w];
				count++;
			}
		}
	}
	if (*size != count) {
//		printf("\n\nchanging size from %d to %d\n\n", *size, count);
		*size = count;
		return realloc(neighbors, (sizeof(struct Square) * count));
	}
	return neighbors;
}

int fieldGetSumOfNeighborFlagsAt(int height, int width) {
	int sum = 0;
	int size = 8;
	struct Square **squares = fieldGetNeighborSquares(height, width, &size);

	for (int i = 0; i < size; i++) {
		if (squares[i]->isFlagged) {
			sum++;
		}
	}

	free(squares);
	return sum;
}

void fieldSetRandomBombs(int total) {
	randomSetupSeed();
	for (int i = 0; i < total; i++) {
		enum boolean setted = false;
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
	if (!fieldIsValidPos(height, width)) {
		return false;
	}
	if (field[height][width].isBomb) {
		return false;
	}

	field[height][width].isBomb = true;
	field[height][width].number = 9;

	int size = 8;

	struct Square** neighbors = fieldGetNeighborSquares(height, width, &size);

	for(int i = 0; i < size; i++){
		neighbors[i]->number++;
	}

	totalBombs++;
	free(neighbors);
	return true;
}

/**
 * @brief prints entire field on terminal
 */
void fieldPrint() {
	printf(" ");
	for (int i = 0; i < fieldWidth; i++) {
		printf(" %d", i);
	}
	printf("\n");
	for (int h = 0; h < fieldHeight; h++) {
		printf("%d[", h);
		for (int w = 0; w < fieldWidth; w++) {
			if (field[h][w].isFlagged) {
				printf("F");
			} else {
				if (field[h][w].isRevealed) {
					if (field[h][w].number >= 9) {
						printf("x");
					} else {
						if (field[h][w].number == 0) {
							printf(" ");
						} else {
							printf("%d", field[h][w].number);
						}
					}
				} else {
					printf("-");
				}
			}

			if (w != fieldWidth - 1) {
				printf("%c", separator);
			} else {
				printf("]\n");
			}
		}
	}
}
