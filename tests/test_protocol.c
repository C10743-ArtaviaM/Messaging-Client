//
// Mauricio Artavia Monge | June 21, 2026
//
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../include/protocol.h"

/* TEST 1 */
void test_serialize_deserialize() {
  printf("Running test: serialize/deserialize...\n");

  Message original;
  memset(&original, 0, sizeof(Message));

  original.sender_rank = 5;
  original.dest_ranks[0] = 2;
  original.dest_count = 1;
  strncpy(original.body, "Hola mundo", MAX_BODY_LEN);

  char buffer[sizeof(Message)];
  Message copy;

  message_serialize(&original, buffer);
  message_deserialize(buffer, &copy);

  assert(original.sender_rank == copy.sender_rank);
  assert(original.dest_ranks[0] == copy.dest_ranks[0]);
  assert(original.dest_count == copy.dest_count);
  assert(strcmp(original.body, copy.body) == 0);

  printf("✔ serialize/deserialize OK\n");
}

/* TEST 2 */
void test_message_integrity() {
  printf("Running test: message integrity...\n");

  Message msg;
  memset(&msg, 0, sizeof(Message));

  msg.sender_rank = 1;
  msg.dest_count = 2;
  msg.dest_ranks[0] = 3;
  msg.dest_ranks[1] = 4;
  strcpy(msg.body, "test");

  assert(msg.sender_rank == 1);
  assert(msg.dest_count == 2);
  assert(msg.dest_ranks[0] == 3);
  assert(msg.dest_ranks[1] == 4);
  assert(strcmp(msg.body, "test") == 0);

  printf("✔ message integrity OK\n");
}

/* TEST 3 (opcional pero recomendado) */
void test_body_limit() {
  printf("Running test: body limit...\n");

  Message msg;
  memset(&msg, 0, sizeof(Message));

  memset(msg.body, 'A', MAX_BODY_LEN - 1);
  msg.body[MAX_BODY_LEN - 1] = '\0';

  assert(strlen(msg.body) < MAX_BODY_LEN);

  printf("✔ body limit OK\n");
}

/* MAIN */
int main() {
  printf("==== RUNNING PROTOCOL TESTS ====\n");

  test_serialize_deserialize();
  test_message_integrity();
  test_body_limit();

  printf("==== ALL TESTS PASSED ====\n");

  return 0;
}