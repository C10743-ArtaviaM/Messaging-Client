//
// Mauricio Artavia Monge | June 21, 2026
//
#ifndef MPI_WRAPPER_H
#define MPI_WRAPPER_H

#include <mpi.h>

#include "../include/protocol.h"

/* Init | Finalize */
/**
 * @brief Inicializacion del entorno MPI con soporte para hilos.
 *
 * @param argc Argumentos del programa.
 * @param argv Argumentos del programa.
 */
void mpi_init(int* argc, char*** argv);

/**
 * @brief Finaliza el entorno MPI.
 */
void mpi_finalize(void);

/* Info */
/**
 * @brief Obtiene el rank del proceso actual.
 *
 * @return Identificador MPI del proceso.
 */
int mpi_rank(void);

/**
 * @brief Obtiene el numero total de procesos MPI.
 *
 * @return Tamano del comunicador MPI_WORLD.
 */
int mpi_size(void);

/* TIME */
/**
 * @brief Obtiene el tiempo actual en segundos desde MPI_Wtime.
 *
 * @return Tiempo en segundos.
 */
double get_time(void);

/* SEND */
/**
 * @brief Envio de un mensaje serializado a otro proceso MPI.
 *
 * @param msg Mensaje a enviar.
 * @param dest Rank destino.
 * @param tag Etiqueta MPI del mensaje.
 */
void send_message(const Message* msg, int dest, int tag);

/* RECV (bloqueante) */
/**
 * @brief Recibe un mensaje de forma bloqueante.
 *
 * @param msg Estructura donde se almacena el mensaje.
 * @param source Rank origen (o MPI_ANY_SOURCE).
 * @param tag Etiqueta MPI (o MPI_ANY_TAG).
 * @param status Estado MPI de la recepcion.
 */
void receive_message(Message* msg, int source, int tag, MPI_Status* status);

/* Try Recv (no bloqueante como con Iprobe) */
/**
 * @brief Recepcion no bloqueante de mensajes.
 *
 * @param msg Estructura donde se almacena el mensaje recibido.
 * @param status Estado MPI del mensaje.
 * @return 1 si se recibio un mensaje, 0 si no hay mensajes dispobibles.
 */
int try_receive_message(Message* msg, MPI_Status* status);

#endif  // MPI_WRAPPER_H