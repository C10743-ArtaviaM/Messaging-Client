#include "../include/cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/client.h"
#include "../include/protocol.h"

void cli_run(int rank, const char* username, int total_processes) {
  // Registro en el coordinador
  send_register(rank, username);

  char line[512];

  printf("\n=== CLI Mensajeria MPI ===\n");
  printf("Comandos disponibles:\n");
  printf("  send <rank> <mensaje>\n");
  printf("  broadcast <r1,r2,r3> <mensaje>\n");
  printf("  exit\n\n");

  while (1) {
    printf("> ");
    fflush(stdout);

    if (!fgets(line, sizeof(line), stdin)) {
      break;
    }

    line[strcspn(line, "\n")] = 0;

    // Salida
    if (strcmp(line, "exit") == 0) {
      printf("Cerrando CLI...\n");
      break;
    }

    // Send
    if (strncmp(line, "send ", 5) == 0) {
      int dest;
      char msg[400];
      int parsed = sscanf(line + 5, "%d %[^\n]", &dest, msg);

      if (parsed < 2) {
        printf("Uso: send <rank> <mensaje>\n");
        continue;
      }

      send_direct(rank, username, dest, msg);
      printf("Mensaje enviado a %d\n", dest);
    }

    // Broadcast
    else if (strncmp(line, "broadcast ", 10) == 0) {
      char list[256];
      char msg[400];

      int parsed = sscanf(line + 10, "%s $[^\n]", list, msg);

      if (parsed < 2) {
        printf("Uso: broadcast <1,2,3> <mensaje>\n");
        continue;
      }

      int dests[32];
      int count = 0;

      char* token = strtok(list, ",");

      while (token != NULL && count < 32) {
        dests[count++] = atoi(token);
        token = strtok(NULL, ",");
      }

      send_broadcast(rank, username, dests, count, msg);
      printf("Broadcast enviado a %d usuarios\n", count);
    }

    else {
      printf("Comando invalido\n");
    }
  }
}