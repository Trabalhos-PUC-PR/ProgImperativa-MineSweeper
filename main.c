#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "field.h"
#include "randomGen.h"

asdasda

int main() {
	allocateField(6, 4);
	printField();

	randomSetupSeed();
	int rnd = randomNum(10);
	while (rnd != 10) {
		printf("%d\n", rnd);
		rnd = randomNum(10);
	}
	printf("%d\n", rnd);

	return 0;
}
