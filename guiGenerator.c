//
// Created by KnightLeo on 10/4/2022.
//

#include "guiGenerator.h"

#include "gtk/gtk.h"
#include "field.h"
#include "square.h"

GtkWidget *game_matrix;
GtkWidget *window;

int square_width = 25;
int square_height = 25;
int square_stroke_width = 3;

double square_spacing = 1.0;

int matrix_margin = 10;

// pega o valor de 0 a 255 e divide por 255
double square_inner_rgb[] = {0.9, 0.9, 0.9};
double square_stroke_rgb[] = {0.3, 0.3, 0.3};
double square_pressed_inner_rgb[] = {0.7, 0.7, 0.7};
double square_flagged_inner_rgb[] = {1.0, 0.0, 0.0}; // temporario
double square_bombed_inner_rgb[] = {0.0, 0.0, 0.0}; // temporario
// deixei assim mesmo pra ficar facil de mudar caso querer

char bombNumberTextColor[8][256] = {"blue", "green", "yellow", "purple", "red", "darkred", "black", "gray"};
// tem uma funcao do gdk que faz parse disso

struct Clickable_shape_Holder{
    struct Square *square;
    GtkWidget *overlayContainer;
};

void newBoard(GtkWidget *parent);

// sem enum pra isso
// 0 = continua o jogo
// 1 = vitoria
// 2 = derrota

int checkGameEnd(struct Square pressedSquare){
    if(pressedSquare.isBomb){
        return 2;
    }
    if(totalSafeSquares == totalSquaresRevealed){
        return 1;
    }
    return 0;
}

void draw_rect(cairo_t *cr, struct Square square){

    cairo_set_source_rgb(cr, square_stroke_rgb[0], square_stroke_rgb[1], square_stroke_rgb[2]);
    cairo_set_line_width(cr, square_stroke_width);
    cairo_rectangle(cr, square_stroke_width*0.5, square_stroke_width*0.5, square_width - square_stroke_width, square_height - square_stroke_width);

    cairo_stroke_preserve(cr);

    cairo_set_source_rgb(cr, square_inner_rgb[0], square_inner_rgb[1], square_inner_rgb[2]);
    if(square.isFlagged){
        // TODO : colocar codigo para bandeira
        cairo_set_source_rgb(cr, square_flagged_inner_rgb[0], square_flagged_inner_rgb[1], square_flagged_inner_rgb[2]);
    }else if(square.isBomb && square.isRevealed){
        // TODO : mostrar bomba no quadrado
        cairo_set_source_rgb(cr, square_bombed_inner_rgb[0], square_bombed_inner_rgb[1], square_bombed_inner_rgb[2]);
    }else if(square.isRevealed){
        cairo_set_source_rgb(cr, square_pressed_inner_rgb[0], square_pressed_inner_rgb[1], square_pressed_inner_rgb[2]);
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
        cairo_set_font_size(cr, square_height*0.5);
        cairo_move_to(cr, square_width*0.3, square_height*0.7);
        cairo_set_source_rgb(cr, color.red, color.green, color.blue);
        cairo_show_text(cr, g_strdup_printf("%d", square.number));
    }
}

static gboolean on_square_draw_event(GtkWidget *widget, cairo_t *cr, struct Square *square){
    draw_rect(cr, *square);
    return FALSE;
}

static gboolean square_on_press(GtkWidget *eventBox, GdkEventButton *event, struct Clickable_shape_Holder clickableShapeHolder){
    if(event->button == 1){ // botao esquerdo do mouse == 1
        fieldRevealAt(clickableShapeHolder.square->yPos, clickableShapeHolder.square->xPos);
    }else{ // botao direito do mouse == 3
        fieldSetFlagAt(clickableShapeHolder.square->yPos, clickableShapeHolder.square->xPos);
    }

    GtkWidget *text;
    GtkTextBuffer *buffer;
    text = gtk_text_view_new();
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
    gtk_text_buffer_set_text(buffer, " ", -1);
    if(clickableShapeHolder.square->yPos > 0){
        gtk_text_buffer_set_text(buffer, g_strdup_printf("%d", clickableShapeHolder.square->number), -1);
    }

    fieldPrint();

    gtk_widget_queue_draw(game_matrix);

    int gameState = checkGameEnd(*clickableShapeHolder.square);
    if(gameState == 1){
        // TODO : mensagem de parabens
    }else if(gameState == 2){
        // TODO : mensagem de derrota
        //newBoard(gtk_widget_get_parent(eventBox));
    }
    return TRUE;
}

void newBoard(GtkWidget *parent){
    game_matrix = gtk_grid_new();

    gtk_widget_destroy(GTK_WIDGET(game_matrix));

    gtk_widget_queue_draw(game_matrix);

    gtk_container_add(GTK_CONTAINER(parent), game_matrix);
    gtk_widget_set_margin_start(game_matrix, matrix_margin);
    gtk_widget_set_margin_top(game_matrix, matrix_margin);
    gtk_widget_set_margin_end(game_matrix, matrix_margin);
    gtk_widget_set_margin_bottom(game_matrix, matrix_margin);
    gtk_widget_set_halign(game_matrix, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(game_matrix, GTK_ALIGN_CENTER);

    for(int i = 0; i < fieldWidth; i++){
        for(int j = 0; j < fieldHeight; j++){
            // cria o container que vai detectar eventos
            GtkWidget *eventBox;
            eventBox = gtk_event_box_new();

            gtk_grid_attach(GTK_GRID(game_matrix), eventBox, i, j, 1, 1);

            // cria o quadrado
            GtkWidget *drawingArea;
            drawingArea = gtk_drawing_area_new();
            gtk_widget_set_size_request(drawingArea, square_width + square_spacing, square_height + square_spacing);
            gtk_container_add(GTK_CONTAINER(eventBox), drawingArea);
            g_signal_connect (G_OBJECT(drawingArea), "draw", G_CALLBACK(on_square_draw_event), &field[j][i]);

            struct Clickable_shape_Holder *clickableShapeHolder = g_malloc0(sizeof(struct Clickable_shape_Holder));
            clickableShapeHolder->square = &field[j][i];
            clickableShapeHolder->overlayContainer = NULL;
            // criar evento pra quando sair o click
            g_signal_connect(G_OBJECT(eventBox), "button_release_event", G_CALLBACK(square_on_press), clickableShapeHolder);
        }
    }
}

void run(int argc, char **argv) {
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *fixed;
    fixed = gtk_fixed_new(); // cria um container pra guardar a matriz, ao inves de so colocar ela direto no top view
    gtk_container_add(GTK_CONTAINER(window), fixed);

    newBoard(fixed);

    gtk_widget_show_all(window);
    gtk_main();
}