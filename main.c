#include <stdio.h>

#include "field.h"
#include "boolean.h"
#include "guiGenerator.h"

void runGame();
void loadGame();

int main(int argc, char **argv){
    loadGame();
    //run(argc, argv);
    runGame();
    return 0;
}

void loadGame(){
    fieldWidth = 5;
    fieldHeight = 5;
    totalBombs = 5;
    allocateField(fieldWidth, fieldHeight);
    fieldSetRandomBombs(totalBombs);
    fieldPrint();
}

void runGame(){
    while (game) {
        enum boolean choice;
        printf("type your choice (flag(0), mark(1)): ");
        scanf("%d", &choice);

        int x;
        int y;
        printf("type the x position (0-%d):", fieldWidth - 1);
        scanf("%d", &x);
        printf("type the y position (0-%d):", fieldHeight - 1);
        scanf("%d", &y);
        if (choice == false) {
            fieldSetFlagAt(y, x);
        } else {
            fieldRevealAt(y, x);
        }
        fieldPrint();
        printf("Safe:[%d/%d] Marked: [%d/%d]\n", totalSquaresRevealed, totalSafeSquares, totalFlaggedSquares, totalBombs);
        printf(" Marked: [%d/%d]\n", totalFlaggedSquares, totalBombs);
        if (choice == true && !fieldIsThisAFlag(y, x) && fieldIsThisABomb(y, x)) {
            game = false;
            fieldRevealAll();
            fieldPrint();
            printf("LOST THE GAME!\n");
        }
        if (totalSafeSquares == totalSquaresRevealed) {
            game = false;
            fieldRevealAll();
            fieldPrint();
            printf("WON THE GAME!\n");
        }
    }
}
