#ifndef UDPSERVER_H
#define UDPSERVER_H


#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h>

#include "definitions.h"

void* myrecv(void);


#endif /* UDPSERVER_H */
