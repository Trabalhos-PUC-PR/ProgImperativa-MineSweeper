#include <stdio.h>
#include "field.h"
#include "boolean.h"

int main() {
	allocateField(9, 9);
	fieldSetRandomBombs(6);
	fieldPrint();
	printf("Safe:[%d/%d] Marked: [%d/%d]\n", totalSquaresRevealed, totalSafeSquares, totalFlaggedSquares, totalBombs);

	while (game) {
		int width;
		int height;
        int choice;
		printf("type your choice (flag(0), mark(1)): ");
		scanf("%d", &choice);

		printf("type the y position (0-%d):", fieldHeight - 1);
		scanf("%d", &height);
		printf("type the x position (0-%d):", fieldWidth - 1);
		scanf("%d", &width);
		if (choice == false) {
			fieldSetFlagAt(height, width);
		} else {
			fieldRevealAt(height, width);
		}
		fieldPrint();
		printf("Safe:[%d/%d] Marked: [%d/%d]\n", totalSquaresRevealed, totalSafeSquares, totalFlaggedSquares, totalBombs);
		if (choice == true && !fieldIsThisAFlag(height, width) && fieldIsThisABomb(height, width)) {
			game = false;
			fieldRevealAll();
			fieldPrint();
			printf("Safe:[%d/%d] Marked: [%d/%d]\n", totalSquaresRevealed, totalSafeSquares, totalFlaggedSquares, totalBombs);
			printf("LOST THE GAME!\n");
		}
		if (totalSafeSquares == totalSquaresRevealed) {
			game = false;
			fieldRevealAll();
			fieldPrint();
			printf("Safe:[%d/%d] Marked: [%d/%d]\n", totalSquaresRevealed, totalSafeSquares, totalFlaggedSquares, totalBombs);
			printf("WON THE GAME!\n");
		}
	}
	return 0;
}
