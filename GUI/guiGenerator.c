//
// Created by KnightLeo on 10/4/2022.
//

#include "guiGenerator.h"

#include "gtk/gtk.h"
#include "field.h"
#include "square.h"

gboolean gameActive;

double square_inner_rgb[] = {0.9, 0.9, 0.9};
double square_stroke_rgb[] = {0.3, 0.3, 0.3};
double square_pressed_inner_rgb[] = {0.7, 0.7, 0.7};
double square_flagged_inner_rgb[] = {1.0, 0.0, 0.0}; // temporario
double square_bombed_inner_rgb[] = {0.0, 0.0, 0.0}; // temporario
double square_toomanyflags_inner_rgb[] = {1, 0.43, 0.44};

// https://www.astrouw.edu.pl/~jskowron/colors-x11/rgb.html
char bombNumberTextColor[8][256] = {"blue", "green", "chocolate1", "purple", "red", "red4", "black", "gray"};

struct GUI {
    GtkWidget *window;
    GtkWidget *game_matrix;
    GtkWidget *game_matrix_container;
} game_gui;

int base_square_size = 30;
int matrix_margin = 25;
double square_stroke_width = 1.0;
int square_spacing = 0;

void addToMatrix();

void draw_rect(cairo_t *cr, struct Square square){
    cairo_set_source_rgb(cr, square_stroke_rgb[0], square_stroke_rgb[1], square_stroke_rgb[2]);
    cairo_set_line_width(cr, square_stroke_width);
    cairo_rectangle(cr, square_stroke_width,square_stroke_width,
                    base_square_size-square_stroke_width,
                    base_square_size-square_stroke_width);
    cairo_stroke_preserve(cr);
    cairo_set_source_rgb(cr, square_inner_rgb[0], square_inner_rgb[1], square_inner_rgb[2]);
    if(square.isFlagged){
        // TODO
        cairo_set_source_rgb(cr, square_flagged_inner_rgb[0], square_flagged_inner_rgb[1], square_flagged_inner_rgb[2]);
    }else if(square.isBomb && square.isRevealed){
        // TODO
        cairo_set_source_rgb(cr, square_bombed_inner_rgb[0], square_bombed_inner_rgb[1], square_bombed_inner_rgb[2]);
    }else if(square.isRevealed){
        cairo_set_source_rgb(cr, square_pressed_inner_rgb[0], square_pressed_inner_rgb[1], square_pressed_inner_rgb[2]);
    }
    if(square.areThereTooManyFlags){
        cairo_set_source_rgb(cr, square_toomanyflags_inner_rgb[0], square_toomanyflags_inner_rgb[1], square_toomanyflags_inner_rgb[2]);
    }
    cairo_fill(cr);
    if(square.isRevealed && square.number > 0 && square.number < 9){
        GdkRGBA color;
        if(!gdk_rgba_parse(&color, bombNumberTextColor[square.number-1])){
            color.red = 0x0000;
            color.green = 0x0000;
            color.blue = 0x0000;
        }
        cairo_select_font_face(cr, "Purisa",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, base_square_size * 0.5);
        cairo_move_to(cr, base_square_size * 0.3, base_square_size * 0.7);
        cairo_set_source_rgb(cr, color.red, color.green, color.blue);
        cairo_show_text(cr, g_strdup_printf("%d", square.number));
    }
}

static gboolean on_square_draw_event(GtkWidget *parent, cairo_t *cr, struct Square *square){
    draw_rect(cr, *square);
    return FALSE;
}

static gboolean square_on_press(GtkWidget *eventBox, GdkEventButton *event, gpointer data){
    struct Square *square = (struct Square *)data;
    if(totalSquaresRevealed == 0){
        fieldFirstClick(square->yPos, square->xPos);
    }
    gboolean continueGame = TRUE;
    if(event->button == 1){
        continueGame = fieldRevealAt(square->yPos, square->xPos);
    }else{
        fieldSetFlagAt(square->yPos, square->xPos);
    }
    gtk_widget_queue_draw(game_gui.game_matrix);
    if(continueGame){
        if(totalSquaresRevealed == totalSafeSquares){
            // TODO
            g_print("\nVENCEU");
        }
    }else{
        // TODO
        g_print("\nPERDEU");
    }
    return TRUE;
}

static gboolean resizeChange(GtkWidget *widget, GdkEvent *event, gpointer userdata){
    int width, height;
    gtk_window_get_size(GTK_WINDOW(game_gui.window), &width, &height);
    if(width > height){
        base_square_size = height/fieldHeight;
    }else{
        base_square_size = width/fieldWidth;
    }
    return FALSE;
}

void drawBoard(GtkWidget *parent){

    GtkAlignment alignment;

    game_gui.game_matrix_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    game_gui.game_matrix = gtk_grid_new();

    gtk_box_pack_start(GTK_BOX(parent), game_gui.game_matrix_container, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(game_gui.game_matrix_container), game_gui.game_matrix, TRUE, TRUE, 0);

    gtk_widget_set_margin_start(game_gui.game_matrix_container, matrix_margin*2 - matrix_margin*0.5);
    gtk_widget_set_margin_top(game_gui.game_matrix_container, matrix_margin);
    gtk_widget_set_margin_bottom(game_gui.game_matrix_container, matrix_margin);

    gtk_grid_set_column_spacing(GTK_GRID(game_gui.game_matrix), square_spacing);
    gtk_grid_set_row_spacing(GTK_GRID(game_gui.game_matrix), square_spacing);

    gtk_grid_set_row_homogeneous(GTK_GRID(game_gui.game_matrix), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(game_gui.game_matrix), TRUE);

    gtk_widget_set_halign(GTK_WIDGET(game_gui.game_matrix), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(game_gui.game_matrix), GTK_ALIGN_FILL);

    addToMatrix();
}

static gboolean onSizeAllocation(GtkWidget *widget, GdkRectangle *allocation, gpointer user_data) {
    int width, height;
    gtk_window_get_size(GTK_WINDOW(game_gui.window), &width, &height);
    if(allocation->width > allocation->height){
        allocation->width = allocation->height;
    }else{
        allocation->height = allocation->width;
    }
    gtk_widget_size_allocate(GTK_WIDGET(widget), allocation);
    return TRUE;
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

            gtk_widget_set_size_request(GTK_WIDGET(drawingArea), -1, -1);

            g_signal_connect (drawingArea, "draw", G_CALLBACK(on_square_draw_event), &field[j][i]);
            g_signal_connect (game_gui.game_matrix, "size-allocate", G_CALLBACK(onSizeAllocation), NULL);
            struct Square *square;
            square = &field[j][i];
            g_signal_connect(G_OBJECT(eventBox), "button_release_event", G_CALLBACK(square_on_press), square);
            gameActive = TRUE;
        }
    }
}

void run(int argc, char **argv) {
    gtk_init(&argc, &argv);

    game_gui.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(game_gui.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(game_gui.window, "configure-event", G_CALLBACK(resizeChange), NULL);

    gtk_window_set_default_size(GTK_WINDOW(game_gui.window), base_square_size*fieldWidth, base_square_size*fieldHeight);

    GtkWidget *box, *menuBar;

    GtkWidget *game, *difficulty, *help; // dropdown menus
    GtkWidget *gameMi, *difficultyMi, *helpMi; // menu items
    GtkWidget *quitMi; // sub menu items
    menuBar = gtk_menu_bar_new();

    game = gtk_menu_new();
    difficulty = gtk_menu_new();
    help = gtk_menu_new();

    game = gtk_menu_new();
    gameMi = gtk_menu_item_new_with_label("Game");
    quitMi = gtk_menu_item_new_with_label("Quit");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(gameMi), game);
    gtk_menu_shell_append(GTK_MENU_SHELL(game), quitMi);
    g_signal_connect(G_OBJECT(quitMi), "activate", G_CALLBACK(gtk_main_quit), NULL);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(game_gui.window), box);

    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), gameMi);

    gtk_box_set_homogeneous(GTK_BOX(box), FALSE);

    gtk_box_pack_start(GTK_BOX(box), menuBar, FALSE, FALSE, 0);

    gtk_window_set_type_hint(GTK_WINDOW(game_gui.window), GDK_WINDOW_TYPE_HINT_DIALOG);

    GdkGeometry geometry;
    geometry.min_aspect = 1;
    geometry.max_aspect = 1;

    gtk_window_set_geometry_hints(GTK_WINDOW(game_gui.window), game_gui.window, &geometry, (GdkWindowHints) GDK_HINT_ASPECT);

    drawBoard(box);
    gtk_widget_show_all(game_gui.window);
    gtk_main();
}