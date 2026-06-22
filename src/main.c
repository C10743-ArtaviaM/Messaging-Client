//
// Mauricio Artavia Monge | June 13, 2026
//
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/cli.h"
#include "../include/client.h"
#include "../include/coordinator.h"
#include "../include/gui.h"
#include "../include/mpi_wrapper.h"

/**
 * @brief Punto de entrada principal del sistema de mensajeria MPI.
 *
 * @param argc Cantidad de argumentos de linea de comandos.
 * @param argv Vector de argumentos de linea de comandos.
 * @return EXIT_SUCCESS si la ejecucion finaliza correctamente,
 *         EXIT_FAILURE si faltan argumentos o configuracion invalida.
 */
int main(int argc, char* argv[]) {
  mpi_init(&argc, &argv);

  int rank = mpi_rank();
  int size = mpi_size();

  if (argc < size) {
    if (rank == 0) {
      fprintf(stderr,
              "Uso: mpirun -np N ./mensajeria user1 user2 ... userN-1\n");
    }
    mpi_finalize();
    return EXIT_FAILURE;
  }

  // Parametros opcionales de prueba: --test <dest> <count>
  int dest = -1;
  int mesg_count = 1;
  int use_gui = 0;
  int use_cli = 0;

  for (int i = size; i < argc; i++) {
    if (strcmp(argv[i], "--test") == 0 && i + 2 < argc) {
      dest = atoi(argv[i + 1]);
      mesg_count = atoi(argv[i + 2]);
    } else if (strcmp(argv[i], "--gui") == 0) {
      use_gui = 1;
    } else if (strcmp(argv[i], "--cli") == 0) {
      use_cli = 1;
    }
  }

  if (rank == 0) {
    coordinator_run(size);
  } else if (use_gui) {
    gui_run(rank, argv[rank], size);
  } else if (use_cli) {
    cli_run(rank, argv[rank], size);
  } else {
    client_run(rank, argv[rank], dest, mesg_count);
  }

  mpi_finalize();
  return EXIT_SUCCESS;
}
