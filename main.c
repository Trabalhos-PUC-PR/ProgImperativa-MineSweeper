#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "field.h"

int main() {
	allocateField(9, 9);

	fieldSetRandomBombs(5);

	fieldRevealAll();
	fieldPrint();

	return 0;
}