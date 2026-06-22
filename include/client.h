//
// Mauricio Artavia Monge | June 13, 2026
//
#ifndef CLIENT_H
#define CLIENT_H

#include "../include/mpi_wrapper.h"

void client_run(int rank, const char* username, int dest, int msg_count);
void send_register(int rank, const char* username);
void send_direct(int rank, const char* username, int dest, const char* text);
void send_broadcast(int rank, const char* username, int* dests, int dest_count,
                    const char* text);

#endif  // CLIENT_H