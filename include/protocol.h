//
// Mauricio Artavia Monge | June 13, 2026
//
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>

#define MAX_BODY_LEN 4096
#define MAX_DEST 64

// Tags MPI para lograr identificar el tipo de mensaje
#define TAG_DIRECT 1     // Mensaje punto a punto.
#define TAG_BROADCAST 2  // Mensaje a multiples puntos.
#define TAG_REGISTER 3   // Registro inicial del cliente en el coordinador.
#define TAG_USER_LIST 4  // Envio de lista de usuarios conectados.

/* Struct de mensaje del sistema */
/** @breif Estructura base de comunicacion del sistema MPI */
typedef struct {
  int sender_rank;           // Rank del proceso emisor.
  int dest_ranks[MAX_DEST];  // Lista de destinatarios.
  int dest_count;            // Cantidad de destinatarios.
  size_t body_len;           // Longitud real del mensaje.
  char body[MAX_BODY_LEN];   // Contenido del mensaje.
} Message;

// Serializacion
/**
 * @brief Serializa un mensaje en un buffer binario.
 *
 * @param msg Puntero al mensaje a serializar.
 * @param buffer Buffer de salida donde se copiara el mensaje.
 */
void message_serialize(const Message* msg, char* buffer);

/**
 * @brief Deserializa un buffer binario en una estructura Message.
 *
 * @param buffer Buffer de entrada con datos serializados.
 * @param msg Puntero al mensaje donde se almacenara el resultado.
 */
void message_deserialize(const char* buffer, Message* msg);

/**
 * @brief Obtiene el tamano fijo del mensaje serializado.
 *
 * @return Tamano en bytes de la estructura Message.
 */
size_t message_size(void);

#endif  // PROTOCOL_H
