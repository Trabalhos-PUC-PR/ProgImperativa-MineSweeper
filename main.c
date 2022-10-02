#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "field.h"

int main() {
	allocateField(9, 9);

	fieldSetRandomBombs(1);

//	fieldSetFlagAt(3, 3);

	fieldRevealAt(0, 0);
	fieldPrint();

	printf("Total revealed: %d | Marked: %d\n",totalSquaresRevealed, totalFlaggedSquares);

	printf("totalSS:%d\n", totalSafeSquares);

	return 0;
}
