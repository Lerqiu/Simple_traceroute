#include "Repository.h"
#include "IcmpHeader.h"
#include "SocketReceive.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static int seqCounter = 0;
static struct icmp **headers;

int Repository_getNextIndex() { return seqCounter; }

bool Reposiotry_isIndexActive(int index) {
  return index <seqCounter && index + RESPONSES >= seqCounter;
}

struct icmp *Repository_get(int index) {
  assert(seqCounter > index && index >= 0);
  return headers[index];
}

struct icmp *Repository_createAndGet() {
  assert(seqCounter < MAX_REPOSITORY_SIZE);
  headers[seqCounter] = malloc(sizeof(struct icmp));
  IcmpHeader_create(headers[seqCounter], seqCounter);
  return headers[seqCounter++];
}

void Repository_init() {
  assert(headers == NULL);
  headers = malloc(sizeof(struct icmp *) * MAX_REPOSITORY_SIZE);
  assert(headers != NULL);
}

void Repository_free() {
  assert(headers != NULL);
  while (--seqCounter >= 0) {
    free(headers[seqCounter]);
  }
  seqCounter = 0;
}