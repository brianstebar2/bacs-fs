/* Sriram */
#ifndef SEND_FILE_H
#define SEND_FILE_H

#include <stdio.h>
#include "UDPclient.h"


#define SIZE 1024

void send_file(char *path, char *IPaddr, uuid_t *uuids, uint64_t num_uuids);


#endif /* SEND_FILE_H */
