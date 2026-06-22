//
// Mauricio Artavia Monge | June 13, 2026
//
#ifndef COORDINATOR_H
#define COORDINATOR_H

#define MAX_CLIENTS 64

#include "../include/mpi_wrapper.h"

/**
 * @struct ClientInfo
 * @brief Informacion basica de un cliente registrado en el sistema.
 */
typedef struct {
  int rank;          /** Rank del proceso MPI */
  char username[32]; /** Nombre del usuario  */
} ClientInfo;

/**
 * @brief Ejecuta la logica principal del coordinador (rank 0).
 *
 * @param total_processes Numero total de procesos MPI en ejecucion.
 */
void coordinator_run(int total_processes);

#endif  // COORDINATOR_H