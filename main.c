#include <stdio.h>
#include <time.h>

#include "field.h"
#include "boolean.h"
#include "GUI/guiGenerator.h"

void loadGame();

enum boolean game = true;

int main(int argc, char **argv){
    loadGame();
    run(argc, argv);
//    runGame();
    return 0;
}

void loadGame(){
    fieldWidth = 9;
    fieldHeight = 9;
    totalBombs = 10;
    allocateField(fieldHeight, fieldWidth);
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
        if (choice == 0) {
            fieldSetFlagAt(y, x);
        } else {
            if(totalSquaresRevealed==0){
                fieldFirstClick(y, x);
                game = fieldRevealAt(y, x);
            }else{
                game = fieldRevealAt(y, x);
            }
        }
        fieldPrint();
        printf("Safe:[%d/%d] Marked: [%d/%d]\n", totalSquaresRevealed, totalSafeSquares, totalFlaggedSquares, totalBombs);
        if (!game || choice == true && !fieldIsThisAFlag(y, x) && fieldIsThisABomb(y, x)) {
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
