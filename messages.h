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

/* Message resource defintions */
#define BLOCK 3
#define FILE 1
#define FOLDER 2
#define SERVER 4

/* Message type definitions */
#define REQUEST 1
#define RESPONSE 2
#define ERROR 3


/* Message header size 
 *  - byte 0: Message action
 *  - byte 1: Message resource
 *  - byte 2: Message type
 */
#define HEADER_SIZE 3



/* ================== *
 * MESSAGE STRUCTURES *
 * ================== */ 



/* ================= *
 * MESSAGE FUNCTIONS *
 * ================= */ 
void create_msg_get_servers_request(char **msg, uint32_t *msg_len);
void create_msg_get_servers_response(char **msg, uint32_t *msg_len);
void create_msg_post_file_request(char *filename, uint64_t file_size, 
                                  char **msg, uint32_t *msg_len);
void create_msg_post_file_response(meta_t *file, char **msg, uint32_t *msg_len);

void generate_header(uint8_t action, uint8_t resource, uint8_t type, char *msg);
uint8_t get_header_action(char *msg);
const char *get_header_action_string(uint8_t action);
uint8_t get_header_resource(char *msg);
const char *get_header_resource_string(uint8_t resource);
uint8_t get_header_type(char *msg);
const char *get_header_type_string(uint8_t resource);

void parse_msg_post_file_request(char *msg, char **filename, uint64_t *file_size);
void parse_msg_post_file_response(char *msg, uuid_t **uuids, uint64_t *num_uuids);



/* =================== *
 * DEBUGGING FUNCTIONS *
 * =================== */ 
void print_msg(char *msg);
void print_msg_post_file_request(char *msg);
void print_msg_post_file_response(char *msg);

#endif /* MESSAGES_H */