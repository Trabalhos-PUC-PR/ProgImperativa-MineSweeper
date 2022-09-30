/*
 * field.h
 *
 *  Created on: Sep 30, 2022
 *      Author: kovalski
 */

#ifndef FIELD_H_
#define FIELD_H_

#include "boolean.h"

void allocateField(int width, int height);
void printField();
enum boolean fieldSetBomb(int x, int y);

#endif /* FIELD_H_ */
