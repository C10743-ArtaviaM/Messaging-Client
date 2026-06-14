//
// Mauricio Artavia Monge | June 13, 2026
//
#include "../include/client.h"

#include <mpi.h>
#include <stdio.h>
#include <string.h>

#include "../include/protocol.h"

static void send_register(int rank, const char* username) {
  Message msg;
  memset(&msg, 0, sizeof(Message));
  msg.sender_rank = rank;
  msg.dest_ranks[0] = 0;  // Coordinador
  msg.dest_count = 1;
  strncpy(msg.body, username, MAX_BODY_LEN - 1);
  msg.body_len = strlen(username);

  char buffer[sizeof(Message)];
  message_serialize(&msg, buffer);
  MPI_Send(buffer, sizeof(Message), MPI_CHAR, 0, TAG_REGISTER, MPI_COMM_WORLD);
  printf("[Cliente %d] Registrado como '%s'\n", rank, username);
}

static void send_direct(int rank, const char* username, int dest,
                        const char* text) {
  Message msg;
  memset(&msg, 0, sizeof(Message));
  msg.sender_rank = rank;
  msg.dest_ranks[0] = dest;
  msg.dest_count = 1;
  strncpy(msg.body, text, MAX_BODY_LEN - 1);
  msg.body_len = strlen(text);

  char buffer[sizeof(Message)];
  message_serialize(&msg, buffer);
  MPI_Send(buffer, sizeof(Message), MPI_CHAR, 0, TAG_DIRECT, MPI_COMM_WORLD);
  printf("[Cliente %d '%s'] Mensaje directo a rank %d: '%s'\n", rank, username,
         dest, text);
}

static void receive_message(int rank) {
  char buffer[sizeof(Message)];
  Message msg;
  MPI_Status status;

  MPI_Recv(buffer, sizeof(Message), MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD,
           &status);
  message_deserialize(buffer, &msg);
  printf("[Cliente %d] Mensaje recibido de rank %d: '%s'\n", rank,
         msg.sender_rank, msg.body);
}

void client_run(int rank, const char* username) {
  send_register(rank, username);

  // Prueba - rank 1 le manda un mensaje a rank 2
  if (rank == 1) {
    send_direct(rank, username, 2, "Hola Usuario!");
  } else {
    receive_message(rank);
  }
}