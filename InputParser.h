#ifndef INPUT_PARSER
#define INPUT_PARSER

#include <arpa/inet.h>

struct sockaddr_in *InputParser(int argc, char *argv[]);

#endif