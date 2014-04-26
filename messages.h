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
#define BACS_BLOCK 3
#define BACS_FILE 1
#define BACS_FOLDER 2
#define BACS_SERVER 4

/* Message type definitions */
#define BACS_REQUEST 1
#define BACS_RESPONSE 2
#define BACS_ERROR 3


/* Message header size 
 *  - byte 0: Message action
 *  - byte 1: Message resource
 *  - byte 2: Message type
 */
#define BACS_HEADER_SIZE 3



/* ================= *
 * MESSAGE FUNCTIONS *
 * ================= */ 

 /* HEADER MANIPULATION */
void generate_header(uint8_t action, uint8_t resource, uint8_t type, char *msg);
uint8_t get_header_action(char *msg);
const char *get_header_action_string(uint8_t action);
uint8_t get_header_resource(char *msg);
const char *get_header_resource_string(uint8_t resource);
uint8_t get_header_type(char *msg);
const char *get_header_type_string(uint8_t resource);


 /* REQUESTS */
void create_msg_delete_file_request(char *filename, char **msg, 
                                    uint64_t *msg_len);
void create_msg_delete_folder_request(char *foldername, char **msg, 
                                      uint64_t *msg_len);
void create_msg_get_block_request(uuid_t uuid, char **msg, uint64_t *msg_len);
void create_msg_get_file_request(char *filename, char **msg, uint64_t *msg_len);
uint8_t create_msg_get_folder_meta_request(char *dirname, char **msg, 
                                           uint64_t *msg_len);
void create_msg_get_servers_request(char **msg, uint64_t *msg_len);
void create_msg_post_block_request(uuid_t uuid, uint32_t size, char *content,
                                   char **msg, uint64_t *msg_len);
void create_msg_post_file_request(char *filename, uint64_t file_size, 
                                  char **msg, uint64_t *msg_len);
void create_msg_post_folder_request(char *foldername, char **msg, 
                                    uint64_t *msg_len);

uint8_t parse_msg_delete_file_request(char *msg, char **filename);
uint8_t parse_msg_delete_folder_request(char *msg, char **foldername);
uint8_t parse_msg_get_block_request(char *msg, uuid_t *uuid);
uint8_t parse_msg_get_file_request(char *msg, char **filename);
uint8_t parse_msg_get_folder_meta_request(char *msg, char **dirname);
uint8_t parse_msg_post_block_request(char *msg, uuid_t *uuid, uint32_t *size, 
                                     char **content);
uint8_t parse_msg_post_file_request(char *msg, char **filename, 
                                    uint64_t *file_size);
uint8_t parse_msg_post_folder_request(char *msg, char **foldername);


/* RESPONSES */
uint8_t create_msg_delete_file_response(char **msg, uint64_t *msg_len);
uint8_t create_msg_delete_folder_response(char **msg, uint64_t *msg_len);
uint8_t create_msg_get_block_response(uuid_t uuid, uint32_t size, char *content,
                                      char **msg, uint64_t *msg_len);
uint8_t create_msg_get_file_response(meta_t *file_meta, char **msg, 
                                     uint64_t *msg_len);
uint8_t create_msg_get_folder_meta_response(meta_t *folder, char **msg, 
                                            uint64_t *msg_len);
void create_msg_get_servers_response(char **msg, uint64_t *msg_len);
uint8_t create_msg_post_block_response(uuid_t uuid, char **msg, 
                                       uint64_t *msg_len);
uint8_t create_msg_post_file_response(meta_t *file, char **msg, 
                                      uint64_t *msg_len);
uint8_t create_msg_post_folder_response(char **msg, uint64_t *msg_len);

void parse_msg_delete_file_response(char *msg);
void parse_msg_delete_folder_response(char *msg);
void parse_msg_get_block_response(char *msg, uuid_t *uuid, uint32_t *size, 
                                  char **content);
void parse_msg_get_file_response(char *msg, basic_block_t **blocks, 
                                 uint64_t *num_blocks);
void parse_msg_get_folder_meta_response(char *msg, basic_meta_t **metas, 
                                        uint32_t *num_metas);
void parse_msg_post_block_response(char *msg, uuid_t *uuid);
void parse_msg_post_file_response(char *msg, uuid_t **uuids, uint64_t *num_uuids);
void parse_msg_post_folder_response(char *msg);


/* ERRORS */
void create_msg_error(uint8_t action, uint8_t resource, uint8_t err_code, 
                      char **msg, uint64_t *msg_len);
uint8_t parse_msg_error(char *msg, char **err_msg);



/* ================ *
 * HELPER FUNCTIONS *
 * ================ */ 
uint8_t msg_with_block(uint8_t action, uint8_t resource, uint8_t type,
                       uuid_t uuid, uint32_t size, char *content,
                       char **msg, uint64_t *msg_len);
uint8_t msg_with_single_element(uint8_t action, uint8_t resource, uint8_t type, 
                                void *element, uint32_t element_len, char **msg, 
                                uint64_t *msg_len);
uint8_t parse_msg_block(char *msg, uint8_t action, uint8_t resource, 
                        uint8_t type, uuid_t *uuid, uint32_t *size, 
                        char **content);
uint8_t parse_msg_single_string(char *msg, uint8_t action, uint8_t resource, 
                                uint8_t type, char **str);



/* =================== *
 * DEBUGGING FUNCTIONS *
 * =================== */ 
void print_msg(char *msg);

/* REQUESTS */
void print_msg_delete_file_request(char *msg);
void print_msg_delete_folder_request(char *msg);
void print_msg_get_block_request(char *msg);
void print_msg_get_file_request(char *msg);
void print_msg_get_folder_meta_request(char *msg);
void print_msg_post_block_request(char *msg);
void print_msg_post_file_request(char *msg);
void print_msg_post_folder_request(char *msg);

/* RESPONSES */
void print_msg_delete_file_response(char *msg);
void print_msg_delete_folder_response(char *msg);
void print_msg_get_block_response(char *msg);
void print_msg_get_file_response(char *msg);
void print_msg_get_folder_meta_response(char *msg);
void print_msg_post_block_response(char *msg);
void print_msg_post_file_response(char *msg);
void print_msg_post_folder_response(char *msg);

/* ERRORS */
void print_msg_error(char *msg);

#endif /* MESSAGES_H */
