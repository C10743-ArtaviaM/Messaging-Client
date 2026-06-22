//
// Mauricio Artavia Monge | June 19, 2026
//
#ifndef GUI_H
#define GUI_H

#include "../include/coordinator.h"

/**
 * @brief Ejecucion de la interfaz grafica del cliente.
 *
 * @param rank Rank MPI del proceso actual.
 * @param username Nombre del usuario asociado a este cliente.
 * @param total_processes Numero total de procesos MPI en ejecucion.
 */
void gui_run(int rank, const char* username, int total_processes);

#endif  // GUI_H