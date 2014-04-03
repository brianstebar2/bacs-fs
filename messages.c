/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#include <stdio.h>


#include <stdlib.h>
#include <string.h>

#include "die_with_error.h"
#include "messages.h"


/**
 * create_msg_get_servers
 *
 * Generates a message string requesting the list of servers in the cluster
 *
 * msg      - (return val) pointer where the message string will be stored
 * msg_len  - (return val) pointer to size of 'msg' string
 * 
 * NOTE: create_msg_get_servers allocates memory for 'msg'; it is the 
 *       responsiblity of the caller to free the allocation
 */
void create_msg_get_servers(char **msg, uint32_t *msg_len)
{
  char *string;
  
  /* Calculate the number of bytes in this message */
  uint32_t num_chars = HEADER_SIZE;

  /* Allocate memory for the message string */
  string = calloc(num_chars, sizeof(char));
  if(string == NULL) die_with_error("create_msg_get_servers - calloc failed");
  memset(string, 0, num_chars*sizeof(char));

  /* Slap a header on the message */
  generate_header(GET, SERVERS, string);

  /* Set the return values */
  *msg_len = num_chars;
  *msg = string;
}



/**
 * create_msg_get_servers_response
 *
 * Generates a message response string containing a list of servers in the 
 * cluster
 *
 * msg      - (return val) pointer where the message string will be stored
 * msg_len  - (return val) pointer to size of 'msg' string
 * 
 * NOTE: create_msg_get_servers_response allocates memory for 'msg'; it is the 
 *       responsiblity of the caller to free the allocation
 */
void create_msg_get_servers_response(char **msg, uint32_t *msg_len)
{
  
}



/**
 * generate_header
 *
 * Stamps a header onto the provided message
 * 
 * action   - action the message is requesting
 * resource - resource the message is targeting
 * msg      - (return val) the message string
 *
 * NOTE: generate_header modifies the first few bytes of the passed-in
 *       msg string
 */
void generate_header(char action, char resource, char *msg)
{
  msg[0] = action;
  msg[1] = resource;
}
