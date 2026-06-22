//
// Mauricio Artavia Monge | June 13, 2026
//
#include "../include/coordinator.h"

#include <mpi.h>
#include <stdio.h>
#include <string.h>

#include "../include/protocol.h"

static ClientInfo clients[MAX_CLIENTS];
static int client_count = 0;

static void register_client(int rank, const char* username) {
  clients[client_count].rank = rank;
  strncpy(clients[client_count].username, username, 31);
  clients[client_count].username[31] = '\0';
  client_count++;
  printf("[Coordinador] Registrado: rank %d -> '%s'\n", rank, username);
}

static void route_direct(Message* msg) {
  int dest = msg->dest_ranks[0];
  char buffer[sizeof(Message)];
  message_serialize(msg, buffer);
  mpiw_send_message(msg, dest, TAG_DIRECT);
  printf("[Coordinador] Mensaje de rank %d -> rank %d\n", msg->sender_rank,
         dest);
}

static void route_broadcast(Message* msg) {
  char buffer[sizeof(Message)];
  message_serialize(msg, buffer);

  for (int i = 0; i < msg->dest_count; i++) {
    int dest = msg->dest_ranks[i];
    mpiw_send_message(msg, dest, TAG_BROADCAST);
  }
  printf("[Coordinador] Broadcast de rank %d -> %d destinatarios\n",
         msg->sender_rank, msg->dest_count);
}

static void broadcast_user_list(int clients_expected) {
  char list_str[MAX_BODY_LEN];
  list_str[0] = '\0';

  for (int i = 0; i < client_count; i++) {
    char entry[64];
    snprintf(entry, sizeof(entry), "%d:%s,", clients[i].rank,
             clients[i].username);
    strncat(list_str, entry, sizeof(list_str) - strlen(list_str) - 1);
  }

  Message msg;
  memset(&msg, 0, sizeof(Message));
  msg.sender_rank = 0;
  strncpy(msg.body, list_str, MAX_BODY_LEN - 1);
  msg.body_len = strlen(list_str);

  char buffer[sizeof(Message)];
  message_serialize(&msg, buffer);

  for (int i = 0; i < clients_expected; i++) {
    mpiw_send_message(&msg, clients[i].rank, TAG_USER_LIST);
  }

  printf("[Coordinador] Lista de usuarios enviada: %s\n", list_str);
}

void coordinator_run(int total_processes) {
  int clients_expected = total_processes - 1;

  // REGISTRO
  for (int i = 0; i < clients_expected; i++) {
    Message msg;
    MPI_Status status;

    mpiw_recv_message(&msg, MPI_ANY_SOURCE, TAG_REGISTER, &status);

    register_client(msg.sender_rank, msg.body);
  }

  printf(
      "[Coordinador] Todos los clientes registrados. Enrutando mensajes ...\n");

  broadcast_user_list(clients_expected);

  // LOOP PRINCIPAL
  while (1) {
    Message msg;
    MPI_Status status;

    if (!mpiw_poll_message(&msg, &status)) {
      continue;
    }

    if (status.MPI_TAG == TAG_DIRECT) {
      route_direct(&msg);
    } else if (status.MPI_TAG == TAG_BROADCAST) {
      route_broadcast(&msg);
    }
  }
}