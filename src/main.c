#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "field.h"
#include "field.c"

int main()
{
    allocateField(20, 20);
    printField();
    return 0;
}