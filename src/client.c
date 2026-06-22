//
// Mauricio Artavia Monge | June 13, 2026
//
#define _GNU_SOURCE

#include "../include/client.h"

#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/protocol.h"

static volatile int keep_listening = 1;

void send_register(int rank, const char* username) {
  Message msg;
  memset(&msg, 0, sizeof(Message));
  msg.sender_rank = rank;
  msg.dest_ranks[0] = 0;  // Coordinador
  msg.dest_count = 1;
  strncpy(msg.body, username, MAX_BODY_LEN - 1);
  msg.body_len = strlen(username);

  char buffer[sizeof(Message)];
  message_serialize(&msg, buffer);
  send_message(&msg, 0, TAG_REGISTER);
  printf("[Cliente %d] Registrado como '%s'\n", rank, username);
}

void send_direct(int rank, const char* username, int dest, const char* text) {
  Message msg;
  memset(&msg, 0, sizeof(Message));
  msg.sender_rank = rank;
  msg.dest_ranks[0] = dest;
  msg.dest_count = 1;
  strncpy(msg.body, text, MAX_BODY_LEN - 1);
  msg.body_len = strlen(text);

  char buffer[sizeof(Message)];
  message_serialize(&msg, buffer);
  send_message(&msg, 0, TAG_DIRECT);
  printf("[Cliente %d '%s'] Mensaje directo a rank %d: '%s'\n", rank, username,
         dest, text);
}

void send_broadcast(int rank, const char* username, int* dests, int dest_count,
                    const char* text) {
  Message msg;
  memset(&msg, 0, sizeof(Message));
  msg.sender_rank = rank;
  msg.dest_count = dest_count;

  for (int i = 0; i < dest_count; i++) {
    msg.dest_ranks[i] = dests[i];
  }

  strncpy(msg.body, text, MAX_BODY_LEN - 1);
  msg.body_len = strlen(text);

  char buffer[sizeof(Message)];
  message_serialize(&msg, buffer);
  send_message(&msg, 0, TAG_BROADCAST);
  printf("[Cliente %d '%s'] Broadcast a %d destinatarios: '%s'\n", rank,
         username, dest_count, text);
}

// Hilo receptor: sondea con MPI_Iprobe sin bloquear, y recibe si hay algo
static void* receiver_thread(void* arg) {
  int rank = *(int*)arg;
  char buffer[sizeof(Message)];
  Message msg;
  MPI_Status status;
  int has_message;

  while (keep_listening) {
    Message msg;
    MPI_Status status;

    if (try_receive_message(&msg, &status)) {
      printf("[Cliente %d] Mensaje recibido de rank %d: '%s'\n", rank,
             msg.sender_rank, msg.body);
    } else {
      usleep(10000);  // 10ms, evita busy-waiting agresivo
    }
  }
  return NULL;
}

// Parsea "2,3" en dests[] = {2,3}, retorna cantidad de destinos
static int parse_dest_list(char* str, int* dests, int max_dests) {
  int count = 0;
  char* token = strtok(str, ",");

  while (token != NULL && count < max_dests) {
    dests[count++] = atoi(token);
    token = strtok(NULL, ",");
  }
  return count;
}

void client_run(int rank, const char* username, int dest, int msg_count) {
  send_register(rank, username);

  pthread_t tid;
  pthread_create(&tid, NULL, receiver_thread, &rank);

  if (dest > 0 && rank != dest) {
    double total_time = 0;

    for (int i = 0; i < msg_count; i++) {
      double start = get_time();
      send_direct(rank, username, dest, "Mensaje de prueba");
      double end = get_time();
      total_time += (end - start);
    }
    printf("[Cliente %d] Latencia promedio (%d mensajes): %f segundos\n", rank,
           msg_count, total_time / msg_count);
  }
  sleep(1);  // Damos tiempo a que el receptor procese lo pendiente
  keep_listening = 0;
  pthread_join(tid, NULL);
}
