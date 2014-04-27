/* Sriram */
#ifndef SEND_FILE_H
#define SEND_FILE_H

#include <stdio.h>
#include "UDPclient.h"
#include "UDPserver.h"


#define SIZE 1024

void send_file(char *path, long IPaddr, uuid_t *uuids, uint64_t num_uuids, int PN);


#endif /* SEND_FILE_H */
