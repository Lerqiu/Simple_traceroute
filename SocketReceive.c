#include "SocketReceive.h"
#include "IcmpHeader.h"
#include "Repository.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

struct icmpResponse {
  long int time_us;
  struct sockaddr_in sender;
};

enum decodeResult { WrongPackage, TTLOut, Reply };

#define msg(...) {if(!printf(__VA_ARGS__)) exit(EXIT_FAILURE);}

static void _printResult(struct icmpResponse responses[], int counter) {
  msg("%i. ", (Repository_getNextIndex() + 1) / RESPONSES); // Grup number

  if (counter == 0) {
    msg("*\n");
    return;
  }

  int average_us = 0;

  // Print unique senders
  for (int outerL = 0; outerL < counter; outerL++) {
    bool unique = true;
    average_us += responses[outerL].time_us;

    char sender_outer[20];
    inet_ntop(AF_INET, &(responses[outerL].sender.sin_addr), sender_outer,
              sizeof(sender_outer));

    for (int innerL = 0; innerL < outerL; innerL++) {
      char sender_inner[20];
      inet_ntop(AF_INET, &(responses[outerL].sender.sin_addr), sender_outer,
                sizeof(sender_outer));
      if (strcmp(sender_inner, sender_outer))
        unique = false;
    }
    if (unique)
      msg("%s ", sender_outer);
  }

  if (counter == RESPONSES)
    msg("%ims\n", average_us / RESPONSES / 1000)
  else
    msg("???\n");
}

static enum decodeResult _decode(int sockfd, struct timeval *timeout,
                                 struct icmpResponse *response) {

  socklen_t sender_len = sizeof(response->sender);
  u_int8_t buffer[IP_MAXPACKET];

  ssize_t packet_len =
      recvfrom(sockfd, buffer, IP_MAXPACKET, 0,
               (struct sockaddr *)&(response->sender), &sender_len);

  if (packet_len < 0) {
    fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  struct ip *ip_header = (struct ip *)buffer;
  ssize_t ip_header_len = 4 * ip_header->ip_hl;

  if (IcmpHeader_isIcmpTimeExceeded(buffer + ip_header_len,
                                    packet_len - ip_header_len, ip_header_len))
    return TTLOut;
  if (IcmpHeader_isReply(buffer + ip_header_len, packet_len - ip_header_len))
    return Reply;
  return WrongPackage;
}

// Return false only when route is found.
bool SocketReceive(int sockfd) {

  fd_set readfds;

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = MAXTIMEOUT_us;

  struct icmpResponse responses[RESPONSES];
  int responseCounter = 0;

  bool doContinue = true;

  while (responseCounter < RESPONSES &&
         (timeout.tv_sec != 0 || timeout.tv_usec != 0)) {

    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    int selectResult = select(sockfd + 1, &readfds, NULL, NULL, &timeout);

    if (selectResult < 0) {
      fprintf(stderr, "ERROR <-> when reading icmp socket: %s\n",
              strerror(errno));
      exit(EXIT_FAILURE);
    }

    if (FD_ISSET(sockfd, &readfds)) {
      enum decodeResult _result =
          _decode(sockfd, &timeout, &responses[responseCounter]);

      // We got some reply. So we mark response time.
      if (_result != WrongPackage) {
        responses[responseCounter++].time_us = MAXTIMEOUT_us - timeout.tv_usec;
        // We found the route
        if (_result == Reply)
          doContinue = false;
      }
    }
  }

  _printResult(responses, responseCounter);

  return doContinue;
}