#ifndef ICMP_HEADER
#define ICMP_HEADER

#include <netinet/ip_icmp.h>
#include <stdbool.h>
#include <sys/types.h>

// Create 'Echo reply'.
void IcmpHeader_create(struct icmp *header, int seqCounter);

// Check if we received 'Time exceeded'.
bool IcmpHeader_isIcmpTimeExceeded(u_int8_t *buffer, int len,
                                   int ip_header_len);

// Check if we received 'Echo reply'.
bool IcmpHeader_isReply(u_int8_t *buffer, int len);

#endif