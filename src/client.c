//
// Mauricio Artavia Monge | June 13, 2026
//
#include "../include/client.h"

#include <stdio.h>

void client_run(int rank, const char* username) {
  printf("Cliente rank %d iniciado como '%s'\n", rank, username);
}