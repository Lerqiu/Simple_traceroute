#include "SocketCreate.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int SocketCreate() {

  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0) {
    fprintf(stderr, "Socket error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in addr;
  bzero((char *)&addr, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    fprintf(stderr, "Binding error %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  
  return sockfd;
}