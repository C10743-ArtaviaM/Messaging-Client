//
// Mauricio Artavia Monge | June 13, 2026
//
#include "../include/protocol.h"

#include <string.h>

void message_serialize(const Message* msg, char* buffer) {
  memcpy(buffer, msg, sizeof(Message));
}

void message_deserialize(const char* buffer, Message* msg) {
  memcpy(msg, buffer, sizeof(Message));
}

size_t message_size(void) { return sizeof(Message); }
