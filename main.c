#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "field.h"
#include "boolean.h"

int main() {
	allocateField(5, 5);
	fieldSetRandomBombs(3);
	fieldPrint();
	printf(" Marked: [%d/%d]\n", totalFlaggedSquares, totalBombs);

	enum choise {
		flag = 0, mark = 1
	} choise;

	while (game) {
		int width;
		int height;
		printf("type your choise (flag(0), mark(1)): ");
		scanf("%d", &choise);

		printf("type the width (0-%d): ", fieldWidth - 1);
		scanf("%d", &width);
		printf("type the height (0-%d): ", fieldHeight - 1);
		scanf("%d", &height);
		if (choise == flag) {
			fieldSetFlagAt(height, width);
		} else {
			fieldRevealAt(height, width);
		}
		fieldPrint();
		printf(" Marked: [%d/%d]\n", totalFlaggedSquares, totalBombs);
		if (choise == mark && !fieldIsThisAFlag(height, width) && fieldIsThisABomb(height, width)) {
			game = false;
			fieldRevealAll();
			printf("LOST THE GAME!\n");
		}
		if (totalSafeSquares == totalSquaresRevealed) {
			game = false;
			fieldRevealAll();
			printf("WON THE GAME!\n");
		}
	}

	return 0;
}
