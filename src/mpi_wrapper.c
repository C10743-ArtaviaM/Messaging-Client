//
// Mauricio Artavia Monge | June 21, 2026
//
#include "../include/mpi_wrapper.h"

#include <string.h>
#include <unistd.h>

static int g_rank = -1;
static int g_size = -1;

/* INIT */
void mpiw_init(int* argc, char*** argv) {
  int provided;
  MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);

  MPI_Comm_rank(MPI_COMM_WORLD, &g_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &g_size);
}

/* Finalize */
void mpiw_finalize(void) { MPI_Finalize(); }

/* INFO */
int mpiw_rank(void) { return g_rank; }

int mpiw_size(void) { return g_size; }

/* Time WRAPPER */
double mpiw_time(void) { return MPI_Wtime(); }

/* SEND WRAPPER */
void mpiw_send_message(const Message* msg, int dest, int tag) {
  char buffer[sizeof(Message)];
  message_serialize(msg, buffer);

  MPI_Send(buffer, sizeof(Message), MPI_CHAR, dest, tag, MPI_COMM_WORLD);
}

/* RECV WRAPPER (bloqueante) */
void mpiw_recv_message(Message* msg, int source, int tag, MPI_Status* status) {
  char buffer[sizeof(Message)];

  MPI_Recv(buffer, sizeof(Message), MPI_CHAR, source, tag, MPI_COMM_WORLD,
           status);

  message_deserialize(buffer, msg);
}

int mpiw_poll_message(Message* msg, MPI_Status* status) {
  int flag = 0;

  MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, status);

  if (!flag) {
    return 0;
  }

  char buffer[sizeof(Message)];

  MPI_Recv(buffer, sizeof(Message), MPI_CHAR, status->MPI_SOURCE,
           status->MPI_TAG, MPI_COMM_WORLD, status);

  message_deserialize(buffer, msg);

  return 1;
}