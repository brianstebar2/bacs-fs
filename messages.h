/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#ifndef MESSAGES_H
#define MESSAGES_H

#include "definitions.h"



/* ========= *
 * CONSTANTS *
 * ========= */
/* Message action definitions (REST-ful) */
#define GET 1
#define POST 2
#define PUT 3
#define DELETE 4

/* Message target resources */
#define BLOCK 3
#define FILE 1
#define FOLDER 2
#define SERVERS 4


/* Message header size 
 *  - byte 0: Message action
 *  - byte 1: Message resource
 */
#define HEADER_SIZE 2



/* ================== *
 * MESSAGE STRUCTURES *
 * ================== */ 



/* ================= *
 * MESSAGE FUNCTIONS *
 * ================= */ 
void create_msg_get_servers(char **msg, uint32_t *msg_len);

void generate_header(char action, char resource, char *msg);



#endif /* MESSAGES_H */