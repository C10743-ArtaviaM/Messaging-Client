//
// Mauricio Artavia Monge | June 13, 2026
//
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/client.h"
#include "../include/coordinator.h"

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (argc < size) {
    if (rank == 0) {
      fprintf(stderr,
              "Uso: mpirun -np N ./mensajeria user1 user2 ... userN-1\n");
    }
    MPI_Finalize();
    return EXIT_FAILURE;
  }

  if (rank == 0) {
    coordinator_run(size);
  } else {
    client_run(rank, argv[rank]);
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}