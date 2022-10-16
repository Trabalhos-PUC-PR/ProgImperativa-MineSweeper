//
// Created by KnightLeo on 10/4/2022.
//

#include "guiGenerator.h"

#include "gtk/gtk.h"
#include "field.h"
#include "square.h"
#include "ctype.h"

double square_inner_rgb[] = {0.9, 0.9, 0.9};
double square_stroke_rgb[] = {0.3, 0.3, 0.3};
double square_pressed_inner_rgb[] = {0.7, 0.7, 0.7};
double square_flagged_inner_rgb[] = {1.0, 0.0, 0.0}; // temporario
double square_bombed_inner_rgb[] = {0.0, 0.0, 0.0}; // temporario
double square_toomanyflags_inner_rgb[] = {1, 0.43, 0.44};

// https://www.astrouw.edu.pl/~jskowron/colors-x11/rgb.html
char bombNumberTextColor[8][256] = {"blue", "green", "chocolate1", "purple", "red", "red4", "black", "gray"};

struct GUI {
    GtkWidget *window, *game_matrix, *game_matrix_container, *main_box;
    int matrix_original_width, matrix_original_height, window_original_height, window_original_width;
} game_gui;

struct BottomBar{
    GtkWidget *bottomMenuBar;
    GtkWidget *timerLabel, *spaceLeftLabel, *flaggedLabel;
} bottomBar;

struct Timer {
    long secondsElapsed;
    enum boolean active;
} timer;

struct CustomDialogEntries {
    GtkWidget *width, *height, *bombs;
};

const int original_square_size = 40;
int square_size = original_square_size;
int matrix_margin = 25;
double square_stroke_width = 1.0;
int square_spacing = 0;

// 1: ongoing game
// 2: game won
// 3: game lost
int gameState;

// width, height, bombs
const int difficulty_easy[] = {9, 9, 10};
const int difficulty_medium[] = {16, 16, 40};
const int difficulty_hard[] = {30, 16, 99};

void addToMatrix();
void createGui();
void updateBottomBarLabels();

void timerStart(){
    timer.secondsElapsed = 0;
    timer.active = true;
}

void timerStop(){
    timer.active = false;
}

void timerReset(){
    timerStop();
    timer.secondsElapsed = 0;
}

void updateTimer(){
    if(timer.active){
        timer.secondsElapsed++;
    }
}

void draw_rect(cairo_t *cr, struct Square square){
    cairo_set_source_rgb(cr, square_stroke_rgb[0], square_stroke_rgb[1], square_stroke_rgb[2]);
    cairo_set_line_width(cr, square_stroke_width);
    cairo_rectangle(cr, square_stroke_width, square_stroke_width,
                    square_size - square_stroke_width,
                    square_size - square_stroke_width);
    cairo_stroke_preserve(cr);
    cairo_set_source_rgb(cr, square_inner_rgb[0], square_inner_rgb[1], square_inner_rgb[2]);
    if(square.isFlagged){
        // TODO
        cairo_set_source_rgb(cr, square_flagged_inner_rgb[0], square_flagged_inner_rgb[1], square_flagged_inner_rgb[2]);
    }else if(square.isBomb && square.isRevealed){
        // TODO
        cairo_set_source_rgb(cr, square_bombed_inner_rgb[0], square_bombed_inner_rgb[1], square_bombed_inner_rgb[2]);
    }else if(square.isRevealed){
        if(square.areThereTooManyFlags){
            cairo_set_source_rgb(cr, square_toomanyflags_inner_rgb[0], square_toomanyflags_inner_rgb[1], square_toomanyflags_inner_rgb[2]);
        }else{
            cairo_set_source_rgb(cr, square_pressed_inner_rgb[0], square_pressed_inner_rgb[1], square_pressed_inner_rgb[2]);
        }
        cairo_fill(cr);
        if(square.number > 0){
            GdkRGBA color;
            if(!gdk_rgba_parse(&color, bombNumberTextColor[square.number-1])){
                color.red = 0x0000;
                color.green = 0x0000;
                color.blue = 0x0000;
            }
            cairo_select_font_face(cr, "",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_BOLD);
            cairo_set_font_size(cr, square_size * 0.9);
            cairo_move_to(cr, square_size * 0.256, square_size * 0.862);
            cairo_set_source_rgb(cr, color.red, color.green, color.blue);
            cairo_show_text(cr, g_strdup_printf("%d", square.number));
        }
        return;
    }
    cairo_fill(cr);
}

static gboolean on_square_draw_event(GtkWidget *parent, cairo_t *cr, struct Square *square){
    draw_rect(cr, *square);
    return FALSE;
}

static gboolean square_on_press(GtkWidget *eventBox, GdkEventButton *event, gpointer data){
    if(gameState != 1){
        return TRUE;
    }
    struct Square *square = (struct Square *)data;
    if(totalSquaresRevealed == 0 && totalFlaggedSquares == 0){
        fieldFirstClick(square->yPos, square->xPos);
        timerStart();
    }
    if(event->button == 1){
        gameState = fieldRevealAt(square->yPos, square->xPos);
    }else{
        fieldSetFlagAt(square->yPos, square->xPos);
    }
    gtk_widget_queue_draw(game_gui.game_matrix);
    if(gameState == 1){
        if(totalSquaresRevealed == totalSafeSquares){
            timerStop();
            gameState = 2;
        }
    }else{
        timerStop();
        gameState = 3;
        g_print("\nPERDEU");
    }
    updateBottomBarLabels();
    return TRUE;
}

static gboolean resizeChange(int width, int height){
    if(width > height){
        square_size = height / fieldHeight;
    }else{
        square_size = width / fieldWidth;
    }
    return FALSE;
}

void centerMatrix(){
    int windowWidth, windowHeight;
    GtkAllocation allocation;
    gtk_widget_get_allocation(GTK_WIDGET(game_gui.game_matrix), &allocation);
    gtk_window_get_size(GTK_WINDOW(game_gui.window), &windowWidth, &windowHeight);
    gtk_widget_set_margin_start(game_gui.game_matrix_container, (windowWidth/2) - allocation.width/2);
    gtk_widget_set_margin_top(game_gui.game_matrix_container, ((windowHeight/2) - allocation.height/2) - matrix_margin);
    gtk_widget_set_margin_bottom(game_gui.game_matrix_container, ((windowHeight/2) - allocation.height/2) + matrix_margin);
}

static gboolean MatrixOnSizeAllocation(GtkWidget *widget, GdkRectangle *allocation, gpointer user_data) {
    int windowWidth, windowHeight;
    gtk_window_get_size(GTK_WINDOW(game_gui.window), &windowWidth, &windowHeight);
    double ratioX = (double) (windowWidth - matrix_margin*2) / (double) game_gui.matrix_original_width;
    double ratioY = (double) (windowHeight - (matrix_margin*2) - matrix_margin) / (double) game_gui.matrix_original_height;
    double ratio;
    if(ratioY > ratioX){
        ratio = ratioX;
    }else{
        ratio = ratioY;
    }
    allocation->width = game_gui.matrix_original_width * ratio;
    allocation->height = game_gui.matrix_original_height * ratio;
    gtk_widget_size_allocate(GTK_WIDGET(widget), allocation);
    resizeChange(allocation->width, allocation->height);
    centerMatrix();
    return TRUE;
}

void drawBoard(GtkWidget *parent){
    game_gui.matrix_original_width = square_size * fieldWidth;
    game_gui.matrix_original_height = square_size * fieldHeight;

    game_gui.game_matrix_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, matrix_margin);

    game_gui.game_matrix = gtk_grid_new();

    gtk_box_pack_start(GTK_BOX(parent), game_gui.game_matrix_container, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(game_gui.game_matrix_container), game_gui.game_matrix, FALSE, TRUE, 0);

    gtk_grid_set_column_spacing(GTK_GRID(game_gui.game_matrix), square_spacing);
    gtk_grid_set_row_spacing(GTK_GRID(game_gui.game_matrix), square_spacing);

    gtk_grid_set_row_homogeneous(GTK_GRID(game_gui.game_matrix), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(game_gui.game_matrix), TRUE);

    gtk_widget_set_halign(GTK_WIDGET(game_gui.game_matrix), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(game_gui.game_matrix), GTK_ALIGN_FILL);
    g_signal_connect (game_gui.game_matrix, "size-allocate", G_CALLBACK(MatrixOnSizeAllocation), NULL);
    addToMatrix();
}

void addToMatrix(){
    for(int i = 0; i < fieldWidth; i++){
        for(int j = 0; j < fieldHeight; j++){
            GtkWidget *eventBox;
            eventBox = gtk_event_box_new();

            gtk_grid_attach(GTK_GRID(game_gui.game_matrix), eventBox, i, j, 1, 1);

            GtkWidget *drawingArea;
            drawingArea = gtk_drawing_area_new();

            gtk_container_add(GTK_CONTAINER(eventBox), drawingArea);

            g_signal_connect (drawingArea, "draw", G_CALLBACK(on_square_draw_event), &field[j][i]);
            struct Square *square;
            square = &field[j][i];
            g_signal_connect(G_OBJECT(eventBox), "button_release_event", G_CALLBACK(square_on_press), square);
        }
    }
}

static gboolean updateTimerLabel(){
    updateTimer();
    long m = timer.secondsElapsed / 60;
    long s = timer.secondsElapsed % 60;
    gtk_menu_item_set_label(GTK_MENU_ITEM(bottomBar.timerLabel), g_strdup_printf("[%02d : %02d]", m, s));
    return TRUE;
}

void resetGame(){
    centerMatrix();
    gameState = 1;
    timerReset();
}

void newBoard(){
    fieldResetField();
    resetGame();
    gtk_widget_queue_draw(GTK_WIDGET(game_gui.game_matrix));
}

void redrawBoard(){
    gtk_widget_destroy(GTK_WIDGET(game_gui.window));
    square_size = original_square_size;
    createGui();
}

enum boolean difficultyChange = false;

void onEasyActivate(){
    difficultyChange = true;
    totalBombs = difficulty_easy[2];
    fieldResizeField(difficulty_easy[1], difficulty_easy[0]);
    redrawBoard();
    difficultyChange = false;
}
void onMediumActivate(){
    difficultyChange = true;
    totalBombs = difficulty_medium[2];
    fieldResizeField(difficulty_medium[1], difficulty_medium[0]);
    redrawBoard();
    difficultyChange = false;
}
void onHardActivate(){
    difficultyChange = true;
    totalBombs = difficulty_hard[2];
    fieldResizeField(difficulty_hard[1], difficulty_hard[0]);
    redrawBoard();
    difficultyChange = false;
}

enum boolean isNumber(const char *string){
    if(string[0] == '\0'){
        return false;
    }
    while (*string){
        if(!isdigit(*string++)){
            return false;
        }
    }
    return true;
}

// -5 = ok
// -6 = cancel
static void onCustomDialogResponse(GtkWidget *widget, gint response_id, gpointer data){
    if(response_id == -5){
        struct CustomDialogEntries* entries = data;
        const char *width = gtk_entry_get_text(GTK_ENTRY(entries->width));
        const char *height = gtk_entry_get_text(GTK_ENTRY(entries->height));
        const char *bombs = gtk_entry_get_text(GTK_ENTRY(entries->bombs));

        if(!isNumber(width) || !isNumber(height) || !isNumber(bombs)){
            return;
        }

        difficultyChange = true;
        totalBombs = atoi(bombs);
        fieldResizeField(atoi(width), atoi(height));
        redrawBoard();
        difficultyChange = false;
        gtk_widget_destroy(widget);
        free(data);
    }else{
        free(data);
        gtk_widget_destroy(widget);
    }
}

void onCustomActivate(){
    GtkWidget *dialog, *contentArea, *grid, *widthLabel, *heightLabel, *bombsLabel;
    struct CustomDialogEntries *customDialogEntries = malloc(sizeof(*customDialogEntries));

    dialog = gtk_dialog_new_with_buttons("Custom Board", GTK_WINDOW(game_gui.window), GTK_DIALOG_MODAL, "Ok", GTK_RESPONSE_OK, "Cancel", GTK_RESPONSE_CANCEL, NULL);
    grid = gtk_grid_new();
    contentArea = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(contentArea), grid);

    customDialogEntries->width = gtk_entry_new();
    customDialogEntries->height = gtk_entry_new();
    customDialogEntries->bombs = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(customDialogEntries->width), g_strdup_printf("%d", fieldWidth));
    gtk_entry_set_text(GTK_ENTRY(customDialogEntries->height), g_strdup_printf("%d", fieldHeight));
    gtk_entry_set_text(GTK_ENTRY(customDialogEntries->bombs), g_strdup_printf("%d", totalBombs));
    widthLabel = gtk_label_new("Width: ");
    heightLabel = gtk_label_new("Height: ");
    bombsLabel = gtk_label_new("Bombs: ");

    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    gtk_grid_attach(GTK_GRID(grid), widthLabel, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), heightLabel, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), bombsLabel, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), customDialogEntries->width, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), customDialogEntries->height, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), customDialogEntries->bombs, 1, 2, 1, 1);

    gtk_widget_show_all(dialog);
    g_signal_connect(GTK_DIALOG(dialog), "response", G_CALLBACK(onCustomDialogResponse), customDialogEntries);

}

void showAboutDialog(){
    GtkWidget *aboutDialog = gtk_about_dialog_new();

    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(aboutDialog), "https://github.com/Trabalhos-PUC-PR/ProgImperativa-MineSweeper");
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(aboutDialog), "MineSweeper");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(aboutDialog), "Simple mineSweeper written in C");

    gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(aboutDialog), "GitHub");

    gtk_dialog_run(GTK_DIALOG(aboutDialog));
    gtk_widget_destroy(aboutDialog);
}

void onDestroyEvent(){
    if(!difficultyChange){
        gtk_main_quit();
    }
}

void updateBottomBarLabels(){
    if(gameState != 1){
        if(gameState == 2){
            gtk_menu_item_set_label(GTK_MENU_ITEM(bottomBar.flaggedLabel), "YOU WIN!");
        }else{
            gtk_menu_item_set_label(GTK_MENU_ITEM(bottomBar.flaggedLabel), "You lose!");
        }
        gtk_menu_item_set_label(GTK_MENU_ITEM(bottomBar.spaceLeftLabel), "");
        return;
    }
    gtk_menu_item_set_label(GTK_MENU_ITEM(bottomBar.flaggedLabel), g_strdup_printf("Marked: %d / %d", totalFlaggedSquares, totalBombs));
    if(totalSafeSquares - totalSquaresRevealed <= totalSafeSquares*0.2){
        gtk_menu_item_set_label(GTK_MENU_ITEM(bottomBar.spaceLeftLabel), g_strdup_printf("%d free space(s) left!", totalSafeSquares - totalSquaresRevealed));
    }

}

void createGui(){
    game_gui.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(game_gui.window, "destroy", G_CALLBACK(onDestroyEvent), NULL);

    game_gui.window_original_height = (square_size * (fieldHeight + 1));
    game_gui.window_original_width = (square_size * fieldWidth) - matrix_margin;

    gtk_window_set_default_size(GTK_WINDOW(game_gui.window), game_gui.window_original_width, game_gui.window_original_height);

    GtkWidget *topMenuBar;

    GtkWidget *gameSeparatorMi = gtk_separator_menu_item_new();
    GtkWidget *difficultySeparatorMi = gtk_separator_menu_item_new();
    GtkWidget *helpSeparatorMi = gtk_separator_menu_item_new();

    GtkWidget *game, *difficulty, *help; // dropdown menus
    GtkWidget *gameMi, *difficultyMi, *helpMi; // menu items
    GtkWidget *quitMi, *newMi; // game sub menu items
    GtkWidget  *easyMi, *mediumMi, *hardMi, *customMi; // difficulty sub menu items
    GtkWidget *aboutMi; // help sub menu items
    topMenuBar = gtk_menu_bar_new();
    bottomBar.bottomMenuBar = gtk_menu_bar_new();

    bottomBar.flaggedLabel = gtk_menu_item_new_with_label(g_strdup_printf("Marked: 0 / %d", totalBombs));
    bottomBar.timerLabel = gtk_menu_item_new_with_label(g_strdup_printf("[00 : 00]"));
    bottomBar.spaceLeftLabel = gtk_menu_item_new_with_label("");

    difficulty = gtk_menu_new();
    difficultyMi = gtk_menu_item_new_with_label("Difficulty");
    easyMi = gtk_menu_item_new_with_label(g_strdup_printf("Easy: %dx%d, %d bombs", difficulty_easy[0], difficulty_easy[1], difficulty_easy[2]));
    mediumMi = gtk_menu_item_new_with_label(g_strdup_printf("Medium: %dx%d, %d bombs", difficulty_medium[0], difficulty_medium[1], difficulty_medium[2]));
    hardMi = gtk_menu_item_new_with_label(g_strdup_printf("Hard: %dx%d, %d bombs", difficulty_hard[0], difficulty_hard[1], difficulty_hard[2]));
    customMi = gtk_menu_item_new_with_label("Custom...");

    help = gtk_menu_new();
    helpMi = gtk_menu_item_new_with_label("Help");
    aboutMi = gtk_menu_item_new_with_label("About");

    game = gtk_menu_new();
    newMi = gtk_menu_item_new_with_label("New");
    gameMi = gtk_menu_item_new_with_label("Game");
    quitMi = gtk_menu_item_new_with_label("Quit");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(gameMi), game);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(difficultyMi), difficulty);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpMi), help);

    gtk_menu_shell_append(GTK_MENU_SHELL(game), newMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(game), gameSeparatorMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(game), quitMi);

    gtk_menu_shell_append(GTK_MENU_SHELL(difficulty), easyMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(difficulty), mediumMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(difficulty), hardMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(difficulty), difficultySeparatorMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(difficulty), customMi);

    gtk_menu_shell_append(GTK_MENU_SHELL(help), helpSeparatorMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(help), aboutMi);

    g_signal_connect(G_OBJECT(quitMi), "activate", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(newMi), "activate", G_CALLBACK(newBoard), NULL);

    g_signal_connect(G_OBJECT(easyMi), "activate", G_CALLBACK(onEasyActivate), NULL);
    g_signal_connect(G_OBJECT(mediumMi), "activate", G_CALLBACK(onMediumActivate), NULL);
    g_signal_connect(G_OBJECT(hardMi), "activate", G_CALLBACK(onHardActivate), NULL);
    g_signal_connect(G_OBJECT(customMi), "activate", G_CALLBACK(onCustomActivate), NULL);

    g_signal_connect(G_OBJECT(aboutMi), "activate", G_CALLBACK(showAboutDialog), NULL);

    game_gui.main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(game_gui.window), game_gui.main_box);

    gtk_menu_shell_append(GTK_MENU_SHELL(topMenuBar), gameMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(topMenuBar), difficultyMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(topMenuBar), helpMi);

    gtk_box_set_homogeneous(GTK_BOX(game_gui.main_box), FALSE);

    gtk_box_pack_start(GTK_BOX(game_gui.main_box), topMenuBar, FALSE, TRUE, 0);

    gtk_menu_shell_append(GTK_MENU_SHELL(bottomBar.bottomMenuBar), bottomBar.timerLabel);
    gtk_menu_shell_append(GTK_MENU_SHELL(bottomBar.bottomMenuBar), bottomBar.flaggedLabel);
    gtk_menu_shell_append(GTK_MENU_SHELL(bottomBar.bottomMenuBar), bottomBar.spaceLeftLabel);

    gtk_box_pack_end(GTK_BOX(game_gui.main_box), bottomBar.bottomMenuBar, FALSE, TRUE, 0);
    drawBoard(game_gui.main_box);
    resetGame();
    gtk_widget_show_all(game_gui.window);
}

void run(int argc, char **argv) {
    gtk_init(&argc, &argv);
    g_timeout_add_seconds(1, updateTimerLabel, NULL);
    createGui();
    gtk_main();
}