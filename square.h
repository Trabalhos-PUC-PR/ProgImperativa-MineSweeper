/*
 * square.h
 *
 *  Created on: Sep 30, 2022
 *      Author: kovalski
 */

#ifndef SQUARE_H_
#define SQUARE_H_

#include "boolean.h"

struct Square{
	int number;
	int xPos;
	int yPos;
	enum boolean isRevealed;
	enum boolean isFlagged;
	enum boolean isBomb;
};

#endif /* SQUARE_H_ */
