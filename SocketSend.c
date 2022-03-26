#include "SocketSend.h"
#include "Repository.h"
#include "SocketReceive.h"
#include <errno.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void _send(int sockfd, struct sockaddr_in *addr) {
  struct icmp *header = Repository_createAndGet();

  ssize_t bytes_sent = sendto(sockfd, header, sizeof(*header), 0,
                              (struct sockaddr *)addr, sizeof(*addr));

  if (bytes_sent == -1) {
    fprintf(stderr, "ERROR <-> icmp package couldn't be send : %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

void SocketSend(int sockfd, struct sockaddr_in *addr, int ttl) {
  setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

  for (int i = 0; i < RESPONSES; i++)
    _send(sockfd, addr);
}