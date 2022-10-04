//
// Created by KnightLeo on 10/4/2022.
//

#include "guiGenerator.h"

#include "gtk/gtk.h"
#include "field.h"

int square_width = 25;
int square_height = 25;
int square_stroke_width = 3;

// pega o valor de 0 a 255 e divide por 255
double square_inner_rgb[] = {0.9, 0.9, 0.9};
double square_stroke_rgb[] = {0.3, 0.3, 0.3};
double square_pressed_rgb[] = {0.5, 0.5, 0.5};
// deixei assim mesmo pra ficar facil de mudar caso querer

double square_spacing = 1.0;

void loadGame(){
    fieldWidth = 5;
    fieldHeight = 5;
    allocateField(fieldWidth, fieldHeight);
    fieldSetRandomBombs(totalBombs);
    fieldPrint();
}

void draw_rect(cairo_t *cr){
    cairo_set_source_rgb(cr, square_stroke_rgb[0], square_stroke_rgb[1], square_stroke_rgb[2]);
    cairo_set_line_width(cr, square_stroke_width);
    cairo_rectangle(cr, square_stroke_width*0.5, square_stroke_width*0.5, square_width - square_stroke_width, square_height - square_stroke_width);
    cairo_stroke_preserve(cr);
    cairo_set_source_rgb(cr, square_inner_rgb[0], square_inner_rgb[1], square_inner_rgb[2]);
    cairo_fill(cr);
}

static gboolean on_square_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data){
    draw_rect(cr);
    gtk_widget_queue_draw_area(widget, 0,0, 25, 234);
    return FALSE;
}

static gboolean square_on_clicked(GtkWidget *eventBox, GdkEventButton *event, gpointer data){
    g_print("CLICADO DEMAIS: %f, %f\n", event->x, event->y);
    cairo_t *drawingArea = data;

    cairo_set_source_rgb(drawingArea, square_pressed_rgb[0], square_pressed_rgb[1], square_pressed_rgb[2]);
    cairo_fill(drawingArea);

    return TRUE;
}

void run(int argc, char **argv) {
    loadGame();
    GtkWidget *window;
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *game_matrix;
    game_matrix = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), game_matrix);
    gtk_widget_set_halign(game_matrix, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(game_matrix, GTK_ALIGN_CENTER);

    for(int i = 0; i < fieldWidth; i++){
        for(int j = 0; j < fieldHeight; j++){
            // cria o container que vai detectar eventos
            GtkWidget *eventBox;
            eventBox = gtk_event_box_new();

            gtk_grid_attach(GTK_GRID(game_matrix), eventBox, i,j,1,1);

            // cria o quadrado
            GtkWidget *drawingArea;
            drawingArea = gtk_drawing_area_new();
            gtk_widget_set_size_request(drawingArea, square_width + square_spacing, square_height + square_spacing);
            gtk_container_add(GTK_CONTAINER(eventBox), drawingArea);
            g_signal_connect (G_OBJECT(drawingArea), "draw", G_CALLBACK(on_square_draw_event), NULL);

            // criar evento pra quando for clicado
            g_signal_connect(G_OBJECT(eventBox), "button_press_event", G_CALLBACK(square_on_clicked), drawingArea);
        }
    }
    gtk_widget_show_all(window);
    gtk_main();
}