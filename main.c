#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "field.h"
#include "randomGen.h"

int main() {
	randomSetupSeed();
	allocateField(9, 9);

	int totalBombs = 35;
	for (int i = 0; i < totalBombs; i++) {
		enum boolean setted = fieldSetBomb(randomNum(fieldHeight - 1),
				randomNum(fieldWidth - 1));
		while (setted == false) {
			setted = fieldSetBomb(randomNum(fieldHeight - 1),
					randomNum(fieldWidth - 1));
		}
	}

	printField();

	return 0;
}
