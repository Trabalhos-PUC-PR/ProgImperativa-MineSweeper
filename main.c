#include <stdio.h>

#include "field.h"
#include "boolean.h"

int main() {
	allocateField(5, 5);
	fieldSetRandomBombs(3);
	fieldPrint();
	printf(" Marked: [%d/%d]\n", totalFlaggedSquares, totalBombs);

	while (game) {
		int width;
		int height;
        enum boolean choice;
		printf("type your choice (flag(0), mark(1)): ");
		scanf("%d", &choice);

		printf("type the x position (0-%d):", fieldWidth - 1);
		scanf("%d", &width);
		printf("type the y position (0-%d):", fieldHeight - 1);
		scanf("%d", &height);
		if (choice == false) {
			fieldSetFlagAt(height, width);
		} else {
			fieldRevealAt(height, width);
		}
		fieldPrint();
		printf(" Marked: [%d/%d]\n", totalFlaggedSquares, totalBombs);
		if (choice == true && !fieldIsThisAFlag(height, width) && fieldIsThisABomb(height, width)) {
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
