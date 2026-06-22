//
// Mauricio Artavia Monge | June 13, 2026
//
#ifndef COORDINATOR_H
#define COORDINATOR_H

#define MAX_CLIENTS 64

#include "../include/mpi_wrapper.h"

typedef struct {
  int rank;
  char username[32];
} ClientInfo;

void coordinator_run(int total_processes);

#endif  // COORDINATOR_H