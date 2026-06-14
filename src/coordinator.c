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
  MPI_Send(buffer, sizeof(Message), MPI_CHAR, dest, TAG_DIRECT, MPI_COMM_WORLD);
  printf("[Coordinador] Mensaje de rank %d -> rank %d\n", msg->sender_rank,
         dest);
}

static void route_broadcast(Message* msg) {
  char buffer[sizeof(Message)];
  message_serialize(msg, buffer);

  for (int i = 0; i < msg->dest_count; i++) {
    int dest = msg->dest_ranks[i];
    MPI_Send(buffer, sizeof(Message), MPI_CHAR, dest, TAG_BROADCAST,
             MPI_COMM_WORLD);
  }
  printf("[Coordinador] Broadcast de rank %d -> %d destinatarios\n",
         msg->sender_rank, msg->dest_count);
}

void coordinator_run(int total_processes) {
  int clients_expected = total_processes - 1;
  char buffer[sizeof(Message)];
  Message msg;
  MPI_Status status;

  // Fase 1 - Registro de Clientes
  for (int i = 0; i < clients_expected; i++) {
    MPI_Recv(buffer, sizeof(Message), MPI_CHAR, MPI_ANY_SOURCE, TAG_REGISTER,
             MPI_COMM_WORLD, &status);
    message_deserialize(buffer, &msg);
    register_client(msg.sender_rank, msg.body);
  }

  printf(
      "[Coordinador] Todos los clientes registrados. Enrutando mensajes ...\n");

  // Fase 2 - Loop de enrutamiento
  while (1) {
    MPI_Recv(buffer, sizeof(Message), MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG,
             MPI_COMM_WORLD, &status);
    message_deserialize(buffer, &msg);

    if (status.MPI_TAG == TAG_DIRECT) {
      route_direct(&msg);
    } else if (status.MPI_TAG == TAG_BROADCAST) {
      route_broadcast(&msg);
    }
  }
}