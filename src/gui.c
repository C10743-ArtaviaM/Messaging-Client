//
// Mauricio Artavia Monge | June 19, 2026
//
#define _GNU_SOURCE

#include "../include/gui.h"

#include <gtk/gtk.h>
#include <mpi.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/client.h"
#include "../include/protocol.h"

static GtkWidget* chat_view;
static GtkWidget* entry;
static int my_rank;
static char my_username[32];
static int dest_rank = -1;  // temp fijo, se setea en gui_run
static volatile int keep_listening = 1;

static void append_to_chat(const char* text) {
  GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_view));
  GtkTextIter end;
  gtk_text_buffer_get_end_iter(buffer, &end);
  gtk_text_buffer_insert(buffer, &end, text, -1);
  gtk_text_buffer_insert(buffer, &end, "\n", -1);
}

// Corre en el hilo principal de GTK, encola con g_idle_add
static gboolean update_chat_from_main_thread(gpointer data) {
  char* text = (char*)data;
  append_to_chat(text);
  free(text);    // Libero la copia en el hilo receptor.
  return FALSE;  // FALSE = ejecucion unica
}

// Hilo receptor: sondea con un MPI_Iprobe, no toca GTK directamente
static void* receiver_thread(void* arg) {
  (void)arg;
  char buffer[sizeof(Message)];
  Message msg;
  MPI_Status status;
  int has_message;

  while (keep_listening) {
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &has_message,
               &status);

    if (has_message) {
      MPI_Recv(buffer, sizeof(Message), MPI_CHAR, status.MPI_SOURCE,
               status.MPI_TAG, MPI_COMM_WORLD, &status);
      message_deserialize(buffer, &msg);

      char* display = malloc(MAX_BODY_LEN + 64);
      snprintf(display, MAX_BODY_LEN + 64, "Rank %d: %s", msg.sender_rank,
               msg.body);

      // Encolo actualizacion que GTK ejecuta en su hilo.
      g_idle_add(update_chat_from_main_thread, display);
    } else {
      usleep(10000);
    }
  }
  return NULL;
}

static void on_send_clicked(GtkWidget* widget, gpointer data) {
  (void)widget;
  (void)data;
  const char* text = gtk_entry_get_text(GTK_ENTRY(entry));

  if (strlen(text) == 0 || dest_rank < 0) {
    return;
  }

  send_direct(my_rank, my_username, dest_rank, text);

  char display[300];
  snprintf(display, sizeof(display), "Yo: %s", text);
  append_to_chat(display);

  gtk_entry_set_text(GTK_ENTRY(entry), "");
}

static void on_window_destroy(GtkWidget* widget, gpointer data) {
  (void)widget;
  (void)data;
  keep_listening = 0;
  gtk_main_quit();
}

void gui_run(int rank, const char* username, int total_processes) {
  my_rank = rank;
  strncpy(my_username, username, sizeof(my_username) - 1);

  // Simplificacion temporal: si soy rank 1, mando a rank 2 y viceversa
  // (remplaza por la lista de contactos proximamente)
  dest_rank = (rank == 1 && total_processes > 2) ? 2 : 1;

  send_register(rank, username);

  // Arranco el hilo recpetor ANTES de entrar al main loop de GTK
  pthread_t tid;
  pthread_create(&tid, NULL, receiver_thread, NULL);

  gtk_init(NULL, NULL);

  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  char title[64];
  snprintf(title, sizeof(title), "Mensajeria - %s (rank %d)", username, rank);
  gtk_window_set_title(GTK_WINDOW(window), title);
  gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
  g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

  GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  chat_view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(chat_view), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(chat_view), GTK_WRAP_WORD);

  GtkWidget* scroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(scroll), chat_view);
  gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);

  GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  entry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 0);

  GtkWidget* send_btn = gtk_button_new_with_label("Enviar");
  g_signal_connect(send_btn, "clicked", G_CALLBACK(on_send_clicked), NULL);
  gtk_box_pack_start(GTK_BOX(hbox), send_btn, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 4);

  gtk_widget_show_all(window);
  gtk_main();

  pthread_join(tid, NULL);
}