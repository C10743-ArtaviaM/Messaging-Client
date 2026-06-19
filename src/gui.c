//
// Mauricio Artavia Monge | June 19, 2026
//
#include "../include/gui.h"

#include <gtk/gtk.h>
#include <string.h>

static GtkWidget* chat_view;
static GtkWidget* entry;

static void on_send_clicked(GtkWidget* widget, gpointer data) {
  (void)widget;
  (void)data;
  const char* text = gtk_entry_get_text(GTK_ENTRY(entry));

  if (strlen(text) == 0) {
    return;
  }

  // Por ahora solo lo muestro en el chat, sin enviar por MPI todavia
  GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_view));
  GtkTextIter end;
  gtk_text_buffer_get_end_iter(buffer, &end);
  gtk_text_buffer_insert(buffer, &end, text, -1);
  gtk_text_buffer_insert(buffer, &end, "\n", -1);

  gtk_entry_set_text(GTK_ENTRY(entry), "");
}

void gui_run(int rank, const char* username, int total_processes) {
  (void)total_processes;

  gtk_init(NULL, NULL);

  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  char title[64];
  snprintf(title, sizeof(title), "Mensajeria - %s (rank %d)", username, rank);
  gtk_window_set_title(GTK_WINDOW(window), title);
  gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  // Area de chat (solo lectura)
  chat_view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(chat_view), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(chat_view), GTK_WRAP_WORD);

  GtkWidget* scroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(scroll), chat_view);
  gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);

  // Fila inferior: entry + boton enviar
  GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  entry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 0);

  GtkWidget* send_btn = gtk_button_new_with_label("Enviar");
  g_signal_connect(send_btn, "clicked", G_CALLBACK(on_send_clicked), NULL);
  gtk_box_pack_start(GTK_BOX(hbox), send_btn, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 4);

  gtk_widget_show_all(window);
  gtk_main();
}