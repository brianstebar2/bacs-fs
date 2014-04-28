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

ErrorCode mysend(void* p, long IPaddr, int PN, int size_of_blocks);
void socket_send_close();
void socket_send_create(int PN);

#endif /* UDPCLIENT_H */
