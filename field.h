/*
 * field.h
 *
 *  Created on: Sep 30, 2022
 *      Author: kovalski
 */

#ifndef FIELD_H_
#define FIELD_H_

#include "boolean.h"

extern enum boolean game;
extern int fieldWidth;
extern int fieldHeight;
extern int totalBombs;
extern int totalSafeSquares;
extern int totalFlaggedSquares;
extern int totalSquaresRevealed;

void allocateField(int width, int height);
void fieldPrint();
enum boolean fieldSetBomb(int x, int y);
enum boolean fieldRevealAt(int height, int width);
void fieldRevealAll();
void fieldSetRandomBombs(int total);
void fieldRevealEmptyField(int height, int width);
enum boolean fieldIsValidPos(int height, int width);
enum boolean fieldSetFlagAt(int height, int width);
enum boolean fieldIsThisABomb(int height, int width);
int fieldGetSumOfNeighborFlagsAt(int height, int width);

#endif /* FIELD_H_ */
