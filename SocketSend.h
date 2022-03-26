#ifndef SOCKET_SEND
#define SOCKET_SEND

#include <netinet/in.h>
#include <sys/types.h>

void SocketSend(int sockfd, struct sockaddr_in *addr, int ttl);

#endif