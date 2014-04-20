/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "die_with_error.h"
#include "messages.h"



/**
 * create_msg_get_servers_request
 *
 * Generates a message string requesting the list of servers in the cluster
 *
 * msg      - (return val) pointer where the message string will be stored
 * msg_len  - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
void create_msg_get_servers_request(char **msg, uint32_t *msg_len)
{
  char *string;
  
  /* Calculate the number of bytes in this message */
  uint32_t num_chars = BACS_HEADER_SIZE;

  /* Allocate memory for the message string */
  string = calloc(num_chars, sizeof(char));
  if(string == NULL) 
    die_with_error("create_msg_get_servers_request - calloc failed");
  memset(string, 0, num_chars*sizeof(char));

  /* Slap a header on the message */
  generate_header(GET, BACS_SERVER, BACS_REQUEST, string);

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
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
void create_msg_get_servers_response(char **msg, uint32_t *msg_len)
{

}



/**
 * create_msg_post_block_request
 *
 * Generates a message string submitting a block
 *
 * uuid    - UUID of the block being uploaded
 * size    - number of bytes in the block
 * content - raw content of the block
 * msg     - (return val) pointer where the message string will be stored
 * msg_len - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
void create_msg_post_block_request(uuid_t uuid, uint32_t size, char *content,
                                   char **msg, uint32_t *msg_len)
{
  char *string;
  uint32_t index;
  
  /* Calculate the number of bytes in this message */
  *msg_len = BACS_HEADER_SIZE +
    sizeof(uuid_t) +    /* length of UUID */
    sizeof(uint32_t) +  /* block size */
    size;               /* size of block */

  /* Allocate memory for the message string and initialize it */
  string = calloc(*msg_len, sizeof(char));
  if(string == NULL) 
    die_with_error("create_msg_post_block_request - calloc failed");
  memset(string, 0, *msg_len * sizeof(char));

  /* Slap a header on the message */
  generate_header(POST, BACS_BLOCK, BACS_REQUEST, string);

  /* Add the block UUID to the message after the header */
  index = BACS_HEADER_SIZE;
  uuid_copy(*(uuid_t *)&string[index], uuid);

  /* Add the block's content after its UUID */
  index = index + sizeof(uuid_t);
  *(uint32_t *)&(string[index]) = size;
  index = index + sizeof(uint32_t);
  strncpy(&string[index], content, size);

  /* TODO: Add checksum to message */

  /* Return the finished message */
  *msg = string;
}



/**
 * create_msg_post_block_response
 *
 * Generates a message response string containing signifying that a block was
 * successfully uploaded
 *
 * uuid     - UUID of the block that was successfully uploaded
 * msg      - (return val) pointer where the message string will be stored
 * msg_len  - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
void create_msg_post_block_response(uuid_t uuid, char **msg, uint32_t *msg_len)
{
  char *string;
  int index;
  
  /* Calculate the number of bytes in this message */
  uint32_t num_chars = BACS_HEADER_SIZE +
    sizeof(uuid_t);   /* Size of UUID */

  /* Allocate memory for the message string */
  string = calloc(num_chars, sizeof(char));
  if(string == NULL) 
    die_with_error("create_msg_post_block_response - calloc failed");
  memset(string, 0, num_chars*sizeof(char));

  /* Slap a header on the message */
  generate_header(POST, BACS_BLOCK, BACS_RESPONSE, string);

  /* Add UUID to string */
  index = BACS_HEADER_SIZE;
  uuid_copy(*(uuid_t *)&string[index], uuid);

  /* Set the return values */
  *msg_len = num_chars;
  *msg = string;
}



/**
 * create_msg_post_file_request
 *
 * Generates a message string submitting the metadata for a new file
 *
 * filename  - string containing the filename of the new file
 * file_size - number of bytes in the new file
 * msg       - (return val) pointer where the message string will be stored
 * msg_len   - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
void create_msg_post_file_request(char *filename, uint64_t file_size, 
                                  char **msg, uint32_t *msg_len)
{
  char *string;
  uint32_t index;
  
  /* Calculate the number of bytes in this message */
  *msg_len = BACS_HEADER_SIZE +
    sizeof(uint16_t) +  /* length of filename */
    strlen(filename) +  /* filename */
    sizeof(uint64_t);   /* size of file */

  /* Allocate memory for the message string and initialize it */
  string = calloc(*msg_len, sizeof(char));
  if(string == NULL) 
    die_with_error("create_msg_post_file_request - calloc failed");
  memset(string, 0, *msg_len * sizeof(char));

  /* Slap a header on the message */
  generate_header(POST, BACS_FILE, BACS_REQUEST, string);

  /* Add the filename to the message after the header */
  index = BACS_HEADER_SIZE;
  *(uint16_t *)&string[index] = (uint16_t)strlen(filename);
  index = index + sizeof(uint16_t);
  strncpy(&string[index], filename, strlen(filename));

  /* Add the file's size after the filename */
  index = index + (strlen(filename) * sizeof(char));
  *(uint64_t *)&(string[index]) = file_size;

  /* Return the finished message */
  *msg = string;
}



/**
 * create_msg_post_file_response
 *
 * Generates a  response string containing a list of block IDs in the file
 *
 * msg      - (return val) pointer where the message string will be stored
 * msg_len  - (return val) pointer to size of 'msg' string
 * 
 * NOTE: the method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
void create_msg_post_file_response(meta_t *file, char **msg, uint32_t *msg_len)
{
  char *string;
  uint64_t index, i;
  uuid_t *uuids;

  /* Calculate the number of bytes in this message */
  *msg_len = BACS_HEADER_SIZE +
    sizeof(uint64_t) +                    /* size of UUIDs list */
    (file->num_blocks * sizeof(uuid_t));  /* list of UUIDs */

  /* Allocate memory for the message string and initalize it */
  string = calloc(*msg_len, sizeof(char));
  if(string == NULL) 
    die_with_error("create_msg_post_file_response - calloc failed");
  memset(string, 0, *msg_len * sizeof(char));

  /* Slap a header on the message */
  generate_header(POST, BACS_FILE, BACS_RESPONSE, string);

  /* Add the length of the UUID list after the header */
  index = BACS_HEADER_SIZE;
  *(uint64_t *)&string[index] = file->num_blocks;

  /* Add UUID list to message */
  index = index + sizeof(uint64_t);
  uuids = (uuid_t *)&string[index];
  for(i=0; i < file->num_blocks; i++) {
    uuid_copy(uuids[i], file->blocks[i]->uuid);
  }

  /* Return the finished message */
  *msg = string;
}



/**
 * generate_header
 *
 * Stamps a header onto the provided message
 * 
 * action   - action the message is requesting
 * resource - resource the message is targeting
 * type     - type of message
 * msg      - (return val) the message string
 *
 * NOTE: generate_header modifies the first few bytes of the passed-in
 *       msg string
 */
void generate_header(uint8_t action, uint8_t resource, uint8_t type, char *msg)
{
  msg[0] = action;
  msg[1] = resource;
  msg[2] = type;
}



/**
 * get_header_action
 *
 * Retrieves the action field from the message's header
 */
uint8_t get_header_action(char *msg)
{
  return msg[0];
}



/**
 * get_header_action_string
 *
 * Translates header action codes into string constants
 */
const char *get_header_action_string(uint8_t action)
{
  switch(action) {
    case GET:     return "GET"; break;
    case POST:    return "POST"; break;
    case PUT:     return "PUT"; break;
    case DELETE:  return "DELTE"; break;
    default:      return "UNKNOWN";
  } 
}



/**
 * get_header_resource
 *
 * Retrieves the resource field from the message's header
 */
uint8_t get_header_resource(char *msg)
{
  return msg[1];
}



/**
 * get_header_resource_string
 *
 * Translates header resource codes into string constants
 */
const char *get_header_resource_string(uint8_t resource)
{
  switch(resource) {
    case BACS_BLOCK:   return "BLOCK"; break;
    case BACS_FILE:    return "FILE"; break;
    case BACS_FOLDER:  return "FOLDER"; break;
    case BACS_SERVER:  return "SERVER"; break;
    default:           return "UNKNOWN";
  } 
}



/**
 * get_header_type
 *
 * Retrieves the type field from the message's header
 */
uint8_t get_header_type(char *msg)
{
  return msg[2];
}



/**
 * get_header_type_string
 *
 * Translates header type codes into string constants
 */
const char *get_header_type_string(uint8_t type)
{
  switch(type) {
    case BACS_REQUEST:   return "REQUEST"; break;
    case BACS_RESPONSE:  return "RESPONSE"; break;
    case BACS_ERROR:     return "ERROR"; break;
    default:             return "UNKNOWN";
  } 
}



/**
 * parse_msg_post_block_request
 *
 * Extracts the UUID, content size, and content of the block from the request
 *
 * msg     - the message to parse
 * uuid    - (return val) pointer to uuid_t where UUID should be written
 * size    - (return val) pointer to number of bytes in the block
 * content - (return val) pointer to a string to contain the block's content
 * 
 * NOTE: this method allocates memory for 'content'; it is the responsiblity 
 *       of the caller to free the allocation
 */
void parse_msg_post_block_request(char *msg, uuid_t *uuid, uint32_t *size, 
                                  char **content)
{
  uint32_t index;
  uuid_t *msg_uuid;
  char *string;

  /* Do a sanity check to make sure we got the expected type of message */
  if(get_header_resource(msg) != BACS_BLOCK || 
     get_header_action(msg) != POST ||
     get_header_type(msg) != BACS_REQUEST)
    die_with_error("parse_msg_post_block_request - invalid message header");

  /* Extract the block's UUID */
  index = BACS_HEADER_SIZE;
  msg_uuid = (uuid_t *)&msg[index];
  uuid_copy(*uuid, *msg_uuid);

  /* Extract the length of the block content from the message */
  index = index + sizeof(uuid_t);
  *size = *(uint32_t *)&msg[index];

  /* Allocate memory for the block content */
  string = calloc(*size, sizeof(char));
  if(string == NULL) 
    die_with_error("parse_msg_post_block_request - calloc failed");
  memset(string, 0, *size * sizeof(char));

  /* Extract content from message */
  index = index + sizeof(uint32_t);
  strncpy(string, &msg[index], *size);
  *content = string;

  /* TODO: Extract checksum from message */
}



/**
 * parse_msg_post_block_response
 *
 * Extracts the UUID of the block from the response
 *
 * msg     - the message to parse
 * uuid    - (return val) pointer to uuid_t where UUID should be written
 */
void parse_msg_post_block_response(char *msg, uuid_t *uuid)
{
  uint32_t index;
  uuid_t *msg_uuid;

  /* Do a sanity check to make sure we got the expected type of message */
  if(get_header_resource(msg) != BACS_BLOCK || 
     get_header_action(msg) != POST ||
     get_header_type(msg) != BACS_RESPONSE)
    die_with_error("parse_msg_post_block_response - invalid message header");

  /* Extract the block's UUID */
  index = BACS_HEADER_SIZE;
  msg_uuid = (uuid_t *)&msg[index];
  uuid_copy(*uuid, *msg_uuid);
}



/**
 * parse_msg_post_file_request
 *
 * Extracts the file name and size of a new file from the request
 *
 * msg       - the message to parse
 * filename  - (return val) pointer to string where filename should be stored
 * file_size - (return val) pointer to number of bytes in the new file
 * 
 * NOTE: this method allocates memory for 'filename'; it is the responsiblity 
 *       of the caller to free the allocation
 */
void parse_msg_post_file_request(char *msg, char **filename, uint64_t *file_size)
{
  char *string;
  uint16_t string_len;
  uint32_t index;

  /* Do a sanity check to make sure we got the expected type of message */
  if(get_header_resource(msg) != BACS_FILE || 
     get_header_action(msg) != POST ||
     get_header_type(msg) != BACS_REQUEST)
    die_with_error("parse_msg_post_file_request - invalid message header");

  /* Extract the length of the filename string from the message; Add one more
     char to the end for a null terminator */
  index = BACS_HEADER_SIZE;
  string_len = *(uint16_t *)&msg[BACS_HEADER_SIZE];

  /* Allocate memory for the filename string */
  string = calloc(string_len + 1, sizeof(char));
  if(string == NULL) 
    die_with_error("parse_msg_post_file_request - calloc failed");
  memset(string, 0, (string_len + 1) * sizeof(char));

  /* Extract filename from message */
  index = index + sizeof(uint16_t);
  strncpy(string, &msg[index], string_len);
  *filename = string;

  /* Extract file size from message */
  index = index + string_len;
  *file_size = *(uint64_t *)&msg[index];
}



/**
 * parse_msg_post_file_request
 *
 * Extracts the file name and size of a new file from the request
 *
 * msg       - the message to parse
 * uuids     - (return val) pointer to where the list of block UUIDs is stored
 * num_uuids - (return val) size of the UUIDs array; If 0 is returned, an error
 *             occurred
 * 
 * NOTE: this method allocates memory for 'uuids'; it is the responsiblity of 
 *       the caller to free the allocation
 */
void parse_msg_post_file_response(char *msg, uuid_t **uuids, uint64_t *num_uuids)
{
  uuid_t *uuid_list, *msg_uuids;
  uint64_t index, i;

  /* Do a sanity check to make sure we got the expected type of message */
  if(get_header_resource(msg) != BACS_FILE || 
     get_header_action(msg) != POST ||
     get_header_type(msg) != BACS_RESPONSE)
    die_with_error("parse_msg_post_file_response - invalid message header");

  /* Extract the number of UUIDs in the response list */
  index = BACS_HEADER_SIZE;
  *num_uuids = *(uint64_t *)&msg[index];

  /* Allocate memory for UUIDs list */
  uuid_list = calloc(*num_uuids, sizeof(uuid_t));
  if(uuid_list == NULL) 
    die_with_error("parse_msg_post_file_response - calloc failed");
  memset(uuid_list, 0, *num_uuids * sizeof(uuid_t));

  /* Extract UUID list */
  index = index + sizeof(uint64_t);
  msg_uuids = (uuid_t *)&msg[index];
  for(i=0; i < *num_uuids; i++) {
    uuid_copy(uuid_list[i], msg_uuids[i]);
  }
  *uuids = uuid_list;
}



/**
 * print_msg
 *
 * DEBUGGING; Prints out the specified message in a human-readable format
 */
void print_msg(char *msg)
{
  printf("%s %s %s: ", 
    get_header_action_string(get_header_action(msg)), 
    get_header_resource_string(get_header_resource(msg)),
    get_header_type_string(get_header_type(msg)));

  switch(get_header_type(msg)) {
    case BACS_REQUEST: 

      switch(get_header_resource(msg)) {
        
        /* BLOCK requests */
        case BACS_BLOCK:  
          switch(get_header_action(msg)) {
            case POST:  print_msg_post_block_request(msg); break;
            default:    printf("INVALID ACTION");
          }

          break;

        /* FILE requests */
        case BACS_FILE:  
          switch(get_header_action(msg)) {
            case POST:  print_msg_post_file_request(msg); break;
            default:    printf("INVALID ACTION");
          }

          break;

        default: printf("UNKNOWN RESOURCE");
      }

      break;

    case BACS_RESPONSE:
      
      switch(get_header_resource(msg)) {
        
        /* BLOCK responses */
        case BACS_BLOCK:  
          switch(get_header_action(msg)) {
            case POST:  print_msg_post_block_response(msg); break;
            default:    printf("INVALID ACTION");
          }

          break;

        /* FILE responses */
        case BACS_FILE:  
          switch(get_header_action(msg)) {
            case POST:  print_msg_post_file_response(msg); break;
            default:    printf("INVALID ACTION");
          }

          break;

        default: printf("UNKNOWN RESOURCE");
      }

      break;      

    default: printf("UNKNOWN MESSAGE TYPE");
  }

  printf("\n");
}



/**
 * print_msg_post_block_request
 *
 * DEBUGGING; Prints out contents of POST BACS_BLOCK request in human-readable 
 * format
 */
void print_msg_post_block_request(char *msg)
{
  uuid_t uuid;
  uint32_t block_size;
  char *content, *str;

  parse_msg_post_block_request(msg, &uuid, &block_size, &content);
  str = uuid_str(uuid);
  printf("%s\n", str);
  free(str);
  printf(" - Content (%d bytes): %s\n", block_size, content);
  free(content);

  /* TODO: Add checksum to message */
}



/**
 * print_msg_post_block_response
 *
 * DEBUGGING; Prints out contents of POST BACS_BLOCK response in human-readable 
 * format
 */
void print_msg_post_block_response(char *msg)
{
  uuid_t uuid;
  char *str;

  parse_msg_post_block_response(msg, &uuid);
  str = uuid_str(uuid);
  printf("%s\n", str);
  free(str);
}



/**
 * print_msg_post_file_request
 *
 * DEBUGGING; Prints out contents of POST BACS_FILE request in human-readable 
 * format
 */
void print_msg_post_file_request(char *msg)
{
  char *filename;
  uint64_t file_size;
  parse_msg_post_file_request(msg, &filename, &file_size);
  printf("%s (%" PRIu64 " bytes)", filename, file_size);
  free(filename);
}



/**
 * print_msg_post_file_response
 *
 * DEBUGGING; Prints out contents of POST BACS_FILE response in human-readable 
 * format
 */
void print_msg_post_file_response(char *msg)
{
  uint64_t num_uuids, i;
  uuid_t *uuids;
  parse_msg_post_file_response(msg, &uuids, &num_uuids);
  
  printf("(%" PRIu64 " blocks):\n", num_uuids);
  for(i = 0; i < num_uuids; i++) {
    char *str = uuid_str(uuids[i]);
    printf(" - %s\n", str);
    free(str);
  }
  
  free(uuids);
}
