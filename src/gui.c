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
#include "../include/coordinator.h"
#include "../include/protocol.h"

static GtkWidget* chat_view;
static GtkWidget* entry;
static GtkWidget* mode_combo;
static GtkWidget* contact_checks[MAX_CLIENTS];
static int contact_ranks[MAX_CLIENTS];
static int contact_count = 0;
static int my_rank;
static char my_username[32];
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

static void on_contact_toggled(GtkToggleButton* toggle, gpointer data) {
  int index = GPOINTER_TO_INT(data);
  int is_broadcast = gtk_combo_box_get_active(GTK_COMBO_BOX(mode_combo)) == 1;

  if (!is_broadcast && gtk_toggle_button_get_active(toggle)) {
    // Modo Directo: desmarcamos todos los demas
    for (int i = 0; i < contact_count; i++) {
      if (i != index) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(contact_checks[i]),
                                     FALSE);
      }
    }
  }
}

static void on_mode_changed(GtkComboBox* combo, gpointer data) {
  (void)data;
  int is_broadcast = gtk_combo_box_get_active(combo) == 1;

  if (!is_broadcast) {
    // Al cambiar a direct, dejamos solo el primero marcado activo
    int found_first = 0;

    for (int i = 0; i < contact_count; i++) {
      gboolean active =
          gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(contact_checks[i]));

      if (active && !found_first) {
        found_first = 1;
      } else if (active) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(contact_checks[i]),
                                     FALSE);
      }
    }
  }
}

static void on_send_clicked(GtkWidget* widget, gpointer data) {
  (void)widget;
  (void)data;
  const char* text = gtk_entry_get_text(GTK_ENTRY(entry));

  if (strlen(text) == 0) {
    return;
  }

  // Recopilamos los contactos que han sido marcados
  int selected[MAX_CLIENTS];
  int selected_count = 0;

  for (int i = 0; i < contact_count; i++) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(contact_checks[i]))) {
      selected[selected_count++] = contact_ranks[i];
    }
  }

  if (selected_count == 0) {
    append_to_chat("(Selecciona al menos un contacto)");
    return;
  }

  int is_broadcast = gtk_combo_box_get_active(GTK_COMBO_BOX(mode_combo)) == 1;

  if (is_broadcast) {
    send_broadcast(my_rank, my_username, selected, selected_count, text);
  } else {
    send_direct(my_rank, my_username, selected[0], text);
  }

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

static void receive_user_list(int rank, char user_list[][40], int* user_count,
                              int* user_ranks) {
  char buffer[sizeof(Message)];
  Message msg;
  MPI_Status status;

  MPI_Recv(buffer, sizeof(Message), MPI_CHAR, 0, TAG_USER_LIST, MPI_COMM_WORLD,
           &status);
  message_deserialize(buffer, &msg);

  *user_count = 0;
  char* token = strtok(msg.body, ",");

  while (token != NULL) {
    int r;
    char name[32];
    sscanf(token, "%d:%31s", &r, name);

    if (r != rank) {
      user_ranks[*user_count] = r;
      strncpy(user_list[*user_count], name, 39);
      (*user_count)++;
    }
    token = strtok(NULL, ",");
  }
}

void gui_run(int rank, const char* username, int total_processes) {
  (void)total_processes;
  my_rank = rank;
  strncpy(my_username, username, sizeof(my_username) - 1);

  // Simplificacion temporal: si soy rank 1, mando a rank 2 y viceversa
  // (remplaza por la lista de contactos proximamente)
  // dest_rank = (rank == 1 && total_processes > 2) ? 2 : 1;

  send_register(rank, username);

  // Recibo la lista de usuarios ANTES de arrancar el hilo receptor, evitamos
  // que compitan por el mismo mensaje.
  char user_list[MAX_CLIENTS][40];
  int user_ranks[MAX_CLIENTS];
  int user_count = 0;
  receive_user_list(rank, user_list, &user_count, user_ranks);

  // Arranco el hilo recpetor ANTES de entrar al main loop de GTK
  pthread_t tid;
  pthread_create(&tid, NULL, receiver_thread, NULL);

  gtk_init(NULL, NULL);

  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  char title[64];
  snprintf(title, sizeof(title), "Mensajeria - %s (rank %d)", username, rank);
  gtk_window_set_title(GTK_WINDOW(window), title);
  gtk_window_set_default_size(GTK_WINDOW(window), 650, 420);
  g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

  GtkWidget* hbox_main = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  gtk_container_add(GTK_CONTAINER(window), hbox_main);

  GtkWidget* contacts_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  gtk_widget_set_size_request(contacts_box, 160, -1);
  GtkWidget* contacts_label = gtk_label_new("Contactos");
  gtk_box_pack_start(GTK_BOX(contacts_box), contacts_label, FALSE, FALSE, 4);

  contact_count = user_count;
  for (int i = 0; i < user_count; i++) {
    contact_ranks[i] = user_ranks[i];
    char label[80];
    snprintf(label, sizeof(label), "%s (rank %d)", user_list[i], user_ranks[i]);
    contact_checks[i] = gtk_check_button_new_with_label(label);
    g_signal_connect(contact_checks[i], "toggled",
                     G_CALLBACK(on_contact_toggled), GINT_TO_POINTER(i));
    gtk_box_pack_start(GTK_BOX(contacts_box), contact_checks[i], FALSE, FALSE,
                       0);
  }
  if (user_count > 0) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(contact_checks[0]), TRUE);
  }

  gtk_box_pack_start(GTK_BOX(hbox_main), contacts_box, FALSE, FALSE, 4);

  GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  gtk_box_pack_start(GTK_BOX(hbox_main), vbox, TRUE, TRUE, 0);

  chat_view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(chat_view), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(chat_view), GTK_WRAP_WORD);

  GtkWidget* scroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(scroll), chat_view);
  gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);

  GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);

  mode_combo = gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_combo), "Directo");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_combo), "Difusion");
  gtk_combo_box_set_active(GTK_COMBO_BOX(mode_combo), 0);
  g_signal_connect(mode_combo, "changed", G_CALLBACK(on_mode_changed), NULL);
  gtk_box_pack_start(GTK_BOX(hbox), mode_combo, FALSE, FALSE, 0);

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