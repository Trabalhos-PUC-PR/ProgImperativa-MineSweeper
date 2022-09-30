/*
 * square.c
 *
 *  Created on: Sep 30, 2022
 *      Author: kovalski
 */

#include "boolean.h"
#include "square.h"

struct Square{
	enum boolean isRevealed;
	enum boolean isFlagged;
	enum boolean isBomb;
	int number;
};
