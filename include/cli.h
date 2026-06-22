//
// Mauricio Artavia Monge | June 22, 2026
//
#ifndef CLI_H
#define CLI_H

/**
 * @brief Ejecucion de la interfaz CLI del cliente.
 *
 * @param rank Identificador MPI del proceso actual.
 * @param username Nombre del usuario asociado al proceso.
 * @param total_processes Numero total de procesos en ejecucion MPI.
 */
void cli_run(int rank, const char* username, int total_processes);

#endif