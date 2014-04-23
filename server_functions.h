/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include "definitions.h"



/* Server message router */
void start_listening();

/* Server logic functions */
void handle_get_block(char *msg, char **response, uint64_t *response_len);
void handle_get_file(char *msg, char **response, uint64_t *response_len);
void handle_get_folder_meta(char *msg, char **response, uint64_t *response_len);
void handle_post_block(char *msg, char **response, uint64_t *response_len);
void handle_post_file(char *msg, char **response, uint64_t *response_len);
void handle_post_folder(char *msg, char **response, uint64_t *response_len);



#endif /* SERVER_FUNCTIONS_H */
