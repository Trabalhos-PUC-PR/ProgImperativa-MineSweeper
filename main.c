#include <stdio.h>
#include <time.h>

#include "field.h"
#include "boolean.h"
#include "GUI/guiGenerator.h"

void runGame();
void loadGame();

enum boolean game = true;
int startTime;
int endTime;

int main(int argc, char **argv){
    loadGame();
    run(argc, argv);
//    runGame();
    return 0;
}

void loadGame(){
    fieldWidth = 6;
    fieldHeight = 5;
    totalBombs = 20;
    startTime = time(NULL);
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
    endTime = time(NULL);
    int seconds = endTime - startTime;
    int minutes = seconds/60;
    seconds = seconds % 60;
    printf("Elapsed: %02d:%02ds\n",minutes, seconds);
}
