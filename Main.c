#include "InputParser.h"
#include "Repository.h"
#include "SocketCreate.h"
#include "SocketReceive.h"
#include "SocketSend.h"
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  struct sockaddr_in *addr = InputParser(argc, argv);
  int sockfd = SocketCreate();

  Repository_init();

  for (int ttl = 1; ttl <= 30; ttl++) {
    SocketSend(sockfd, addr, ttl);

    // Check if we received reply from chosen address.
    if (!SocketReceive(sockfd))
      break;
  }

  Repository_free();
  free(addr);
  close(sockfd);

  return EXIT_SUCCESS;
}
