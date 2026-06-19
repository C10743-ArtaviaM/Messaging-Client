//
// Mauricio Artavia Monge | June 13, 2026
//
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/client.h"
#include "../include/coordinator.h"

int main(int argc, char* argv[]) {
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

  if (provided < MPI_THREAD_MULTIPLE) {
    fprintf(stderr,
            "Advertencia: se solicita MPI_THREAD_MULTIPLE pero el nivel "
            "optenido fue %d\n",
            provided);
  }

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

  // Parametros opcionales de prueba: --test <dest> <count>
  int dest = -1;
  int mesg_count = 1;

  for (int i = size; i < argc - 2; i++) {
    if (strcmp(argv[i], "--test") == 0) {
      dest = atoi(argv[i + 1]);
      mesg_count = atoi(argv[i + 2]);
      break;
    }
  }

  if (rank == 0) {
    coordinator_run(size);
  } else {
    client_run(rank, argv[rank], dest, mesg_count);
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}
