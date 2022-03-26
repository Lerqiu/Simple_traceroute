#include "InputParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

struct sockaddr_in *InputParser(int argc, char *argv[]) {

  struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
  bzero(addr, sizeof(*addr));
  addr->sin_family = AF_INET;

  if (argc != 2 || inet_pton(AF_INET, argv[1], &(addr->sin_addr)) != 1) {
    fprintf(stderr,"ERROR <-> program take only one ipv4 address as parameter\n");
    exit(EXIT_FAILURE);
  }

  return addr;
}