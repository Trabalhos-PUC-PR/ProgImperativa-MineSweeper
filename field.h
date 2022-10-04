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

void fieldPrint();
void fieldRevealAll();
void fieldSetRandomBombs(int total);
enum boolean fieldSetBomb(int x, int y);
void allocateField(int width, int height);
enum boolean fieldRevealAt(int height, int width);
void fieldRevealEmptyField(int height, int width);
enum boolean fieldSetFlagAt(int height, int width);
enum boolean fieldIsValidPos(int height, int width);
enum boolean fieldIsThisAFlag(int height, int width);
enum boolean fieldIsThisABomb(int height, int width);
enum boolean fieldRevealAround(int height, int width);
int fieldGetSumOfNeighborFlagsAt(int height, int width);

#endif /* FIELD_H_ */
