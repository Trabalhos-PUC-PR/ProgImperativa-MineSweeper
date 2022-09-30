/*
 * randomGen.c
 *
 *  Created on: Sep 30, 2022
 *      Author: kovalski
 */
#include <time.h>
#include <stdlib.h>
#include "randomGen.h"

/**
 * @brief sets up a random seed based on current time,
 * make sure to call this function at least once to
 * ensure pseudo-random numbers
 */
void randomSetupSeed() {
	srand(time(NULL));
}

/**
 * @brief returns a random number between 0 and the specified limit
 */
int randomNum(int limit) {
	return rand() % (limit + 1);
}

