#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "field.h"
#include "randomGen.h"

int main() {
	randomSetupSeed();
	allocateField(3, 3);

	printf("success %d\n", fieldSetBomb(2, 2));

	printField();

	return 0;
}
