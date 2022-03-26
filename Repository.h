#ifndef REPOSITORY
#define REPOSITORY

#include <stdbool.h>

#include "Main.h"

void Repository_init();
void Repository_free();

struct icmp *Repository_createAndGet();
struct icmp *Repository_get(int index);

int Repository_getNextIndex();
bool Reposiotry_isIndexActive(int index);

#endif