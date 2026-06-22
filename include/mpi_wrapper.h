//
// Mauricio Artavia Monge | June 21, 2026
//
#ifndef MPI_WRAPPER_H
#define MPI_WRAPPER_H

#include <mpi.h>

#include "../include/protocol.h"

/* Init | Finalize */
void mpiw_init(int* argc, char*** argv);
void mpiw_finalize(void);

/* Info */
int mpi_rank(void);
int mpi_size(void);

/* TIME */
double mpiw_time(void);

/* SEND */
void mpiw_send_message(const Message* msg, int dest, int tag);

/* RECV (bloqueante) */
void mpiw_recv_message(Message* msg, int source, int tag, MPI_Status* status);

/* POLL (no bloquante como con Iprobe) */
int mpiw_poll_message(Message* msg, MPI_Status* status);

#endif  // MPI_WRAPPER_H