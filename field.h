/*
 * field.h
 *
 *  Created on: Sep 30, 2022
 *      Author: kovalski
 */

#ifndef FIELD_H_
#define FIELD_H_

#include "boolean.h"

extern int fieldWidth;
extern int fieldHeight;
extern int totalPlacedBombs;

void allocateField(int width, int height);
void fieldPrint();
enum boolean fieldSetBomb(int x, int y);
enum boolean fieldRevealAt(int height, int width);
void fieldRevealAll();
void fieldSetRandomBombs(int total);

#endif /* FIELD_H_ */
