//
// Mauricio Artavia Monge | June 21, 2026
//
#ifndef MPI_WRAPPER_H
#define MPI_WRAPPER_H

#include <mpi.h>

#include "../include/protocol.h"

/* Init | Finalize */
void mpi_init(int* argc, char*** argv);
void mpi_finalize(void);

/* Info */
int mpi_rank(void);
int mpi_size(void);

/* TIME */
double get_time(void);

/* SEND */
void send_message(const Message* msg, int dest, int tag);

/* RECV (bloqueante) */
void receive_message(Message* msg, int source, int tag, MPI_Status* status);

/* POLL (no bloquante como con Iprobe) */
int try_receive_message(Message* msg, MPI_Status* status);

#endif  // MPI_WRAPPER_H