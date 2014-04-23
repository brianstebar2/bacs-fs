#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h>

#include "definitions.h"

ErrorCode mysend(void* p, char* IPaddr, int PN, int size_of_blocks);

#endif /* UDPCLIENT_H */
