//
// Mauricio Artavia Monge | June 13, 2026
//
#ifndef CLIENT_H
#define CLIENT_H

#include "../include/mpi_wrapper.h"

/**
 * @brief Ejecucion de la logica principal del cliente en modo CLI o test.
 *
 * @param rank Identificador del proceso MPI.
 * @param username Nombre del usuario asociado al cliente.
 * @param dest Destino de prueba para envio de mensajes (-1 si no aplicara).
 * @param msg_count Cantidad de mensajes a enviar en modo test.
 */
void client_run(int rank, const char* username, int dest, int msg_count);

/**
 * @brief Registro del cliente en el coordinador MPI.
 *
 * @param rank Identificador del proceso MPI.
 * @param username Nombre del usuario.
 */
void send_register(int rank, const char* username);

/**
 * Envio de un mensaje directo a un cliente especifico.
 * @param rank Identificador del proceso emisor.
 * @param username Nombre del usuario emisor.
 * @param dest Rank del destinatario.
 * @param text Contenido del mensaje.
 */
void send_direct(int rank, const char* username, int dest, const char* text);

/**
 * @brief Envio de un mensaje de difusion a multiples clientes.
 *
 * @param rank Identificador del proceso emisor.
 * @param username Nombre del usuario emisor.
 * @param dests Arreglo de ranks destino.
 * @param dest_count Cantidad de destinatarios.
 * @param text Contenido del mensaje.
 */
void send_broadcast(int rank, const char* username, int* dests, int dest_count,
                    const char* text);

#endif  // CLIENT_H