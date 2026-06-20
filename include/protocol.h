//
// Mauricio Artavia Monge | June 13, 2026
//
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>

#define MAX_BODY_LEN 4096
#define MAX_DEST 64

// Tags MPI para lograr identificar el tipo de mensaje

#define TAG_DIRECT 1
#define TAG_BROADCAST 2
#define TAG_REGISTER 3
#define TAG_USER_LIST 4

typedef struct {
  int sender_rank;
  int dest_ranks[MAX_DEST];
  int dest_count;
  size_t body_len;
  char body[MAX_BODY_LEN];
} Message;

// Serializacion
void message_serialize(const Message* msg, char* buffer);
void message_deserialize(const char* buffer, Message* msg);
size_t message_size(void);

#endif  // PROTOCOL_H
