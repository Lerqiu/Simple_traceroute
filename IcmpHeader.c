#include "IcmpHeader.h"
#include "Repository.h"
#include <arpa/inet.h>
#include <assert.h>
#include <bits/types.h>
#include <endian.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

// ICMP code field contains seq number and part of pid
// 10 major bits belong to seq
static u_int16_t _getSeq(u_int16_t seq) { 
  return seq >> 6; 
}
static u_int64_t _getPid(u_int16_t id, u_int16_t seq) {
  return (u_int64_t)id | (((u_int64_t)(seq & 0x03F)) << 16);
}

// ICMP code field contains seq number and part of pid
// 10 major bits belong to seq
static u_int64_t _createSeq(__pid_t pid, int seq) {
  return ((pid >> 16) & 0x03F) | (seq << 6);
}
static u_int64_t _createID(__pid_t pid) { 
  return pid & 0x0FFFF; 
}

static u_int16_t _checksum_compute(const void *buff, int length) {
  u_int32_t sum;
  const u_int16_t *ptr = buff;
  assert(length % 2 == 0);
  for (sum = 0; length > 0; length -= 2)
    sum += *ptr++;
  sum = (sum >> 16) + (sum & 0xffff);
  return (u_int16_t)(~(sum + (sum >> 16)));
}

// Create 'Echo reply'
void IcmpHeader_create(struct icmp *header, int seqCounter) {
  header->icmp_type = ICMP_ECHO;
  header->icmp_code = 0;
  header->icmp_hun.ih_idseq.icd_id = htobe16(_createID(getpid()));
  header->icmp_hun.ih_idseq.icd_seq = htobe16(_createSeq(getpid(), seqCounter));
  header->icmp_cksum = 0;
  header->icmp_cksum = _checksum_compute((u_int16_t *)header, sizeof(*header));
}

// Check type and code field in ICMP.
bool _isType(u_int8_t *buffer, int len, int type, int code) {
  if (len < 8)
    return false;

  u_int8_t IType = *buffer;
  u_int8_t ICode = *(buffer + 1);

  return IType == type && ICode == code;
}

// Check if we received 'Echo reply'.
bool IcmpHeader_isReply(u_int8_t *buffer, int len) {
  if (!_isType(buffer, len, 0, 0))
    return false;

  u_int16_t IOID = be16toh(*((u_int16_t *)(buffer + 4)));
  u_int16_t IOSeq = be16toh(*((u_int16_t *)(buffer + 6)));

  return (_getPid(IOID, IOSeq) == getpid() && 
          Reposiotry_isIndexActive(_getSeq(IOSeq)));
}

// Check if we received 'Time exceeded'.
bool IcmpHeader_isIcmpTimeExceeded(u_int8_t *buffer, int len, int ip_header_len) {
  if (!(_isType(buffer, len, 11, 0) && len >= 8 + ip_header_len + 8))
    return false;

  u_int8_t *ODataGram = buffer + ip_header_len + 8;

  u_int8_t IOType = *ODataGram;
  u_int8_t IOCode = *(ODataGram + 1);
  u_int16_t IOCSum = *((u_int16_t *)(ODataGram + 2));
  u_int16_t IOID = *((u_int16_t *)(ODataGram + 4));
  u_int16_t IOSeq = *((u_int16_t *)(ODataGram + 6));

  int index = _getSeq(be16toh(IOSeq));
  if (!Reposiotry_isIndexActive(index))
    return false;

  struct icmp *header = Repository_get(index);

  return (IOType == header->icmp_type && IOCode == header->icmp_code &&
          IOCSum == header->icmp_cksum &&
          IOID == header->icmp_hun.ih_idseq.icd_id &&
          IOSeq == header->icmp_hun.ih_idseq.icd_seq);
}