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
#include "errors.h"
#include "messages.h"



/**
 * create_msg_error
 *
 * Generates a message string containing an error message
 *
 * action   - action of the erroring request
 * resource - resource of the erroring request
 * err_msg  - null-terminated string containing the error message
 * msg      - (return val) pointer where the message string will be stored
 * msg_len  - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
void create_msg_error(uint8_t action, uint8_t resource, uint8_t err_code, 
                      char **msg, uint64_t *msg_len)
{
  uint8_t status;
  status = msg_with_single_element(action, resource, BACS_ERROR, 
    (char *)error_message(err_code), strlen(error_message(err_code)), 
    msg, msg_len);

  if(status != NO_ERROR) 
    die_with_error("create_msg_error - msg_with_single_element failed");
}



/**
 * create_msg_get_block_request
 *
 * Generates a message string requesting to download a block
 *
 * uuid     - UUID of the block to request
 * msg      - (return val) pointer where the message string will be stored
 * msg_len  - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
void create_msg_get_block_request(uuid_t uuid, char **msg, uint64_t *msg_len)
{
  msg_with_single_element(GET, BACS_BLOCK, BACS_REQUEST, uuid, sizeof(uuid_t), 
                          msg, msg_len);
}



/**
 * create_msg_get_block_response
 *
 * Generates a message string return a block; Returns NO_ERROR or an error code
 *
 * uuid    - UUID of the block being returned
 * size    - number of bytes in the block
 * content - raw content of the block
 * msg     - (return val) pointer where the message string will be stored
 * msg_len - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
uint8_t create_msg_get_block_response(uuid_t uuid, uint32_t size, char *content,
                                      char **msg, uint64_t *msg_len)
{
  return msg_with_block(GET, BACS_BLOCK, BACS_RESPONSE, uuid, size, content, 
                 msg, msg_len);
}



/**
 * create_msg_get_file_request
 *
 * Generates a message string requesting to download a file
 *
 * filename - null-terminated string containing the fully-qualified path of the
 *            target file
 * msg      - (return val) pointer where the message string will be stored
 * msg_len  - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
void create_msg_get_file_request(char *filename, char **msg, uint64_t *msg_len)
{
  msg_with_single_element(GET, BACS_FILE, BACS_REQUEST, 
                          filename, (uint32_t)strlen(filename), msg, msg_len);
}



/**
 * create_msg_get_file_response
 *
 * Generates a message response string include the list of the file's block 
 * UUIDs and the list of server IPs storing each block; Returns NO_ERROR or an
 * error code
 *
 * file_meta - meta_t for the file containing the metadata to send
 * msg       - (return val) pointer where the message string will be stored
 * msg_len   - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
uint8_t create_msg_get_file_response(meta_t *file_meta, char **msg, 
                                     uint64_t *msg_len)
{
  char *string;
  uint64_t index, i;
  block_t *current;

  /* Calculate the number of bytes in the message */
  uint64_t num_chars = BACS_HEADER_SIZE +
    sizeof(uint64_t);  /* Number of basic_block_t's in the message */

  /* Add the bytes needed for each basic_block_t */
  for(i=0; i < file_meta->num_blocks; i++) {
    current = file_meta->blocks[i];

    num_chars = num_chars +
      sizeof(uuid_t) +        /* Block UUID */
      /*sizeof(uint32_t) +*/      /* Block checksum */
      sizeof(uint8_t);        /* Number of block hosts */
      /*current->num_hosts*sizeof(uint32_t);*/ /* Block host IPs */
  }

  /* Allocate memory for the message */
  string = calloc(num_chars, sizeof(char));
  if(string == NULL) return ERR_MEM_ALLOC;
  memset(string, 0, num_chars*sizeof(char));

  /* Slap a header on the message */
  generate_header(GET, BACS_FILE, BACS_RESPONSE, string);

  /* Add the count of basic_block_t's to the message */
  index = BACS_HEADER_SIZE;
  *(uint64_t *)&string[index] = file_meta->num_blocks;
  index = index + sizeof(uint64_t);

  /* Add each block's metadata to the message */
  for(i=0; i < file_meta->num_blocks; i++) {
    current = file_meta->blocks[i];

    /* Add the block UUID */
    uuid_copy(*(uuid_t *)&string[index], current->uuid);
    index = index + sizeof(uuid_t);

    /* TODO: Add the block's checksum */

    /* TODO: Add the block's host IPs */
    *(uint8_t *)&string[index] = 0;
    index = index + sizeof(uint8_t);
  }

  /* Set the return values */
  *msg_len = num_chars;
  *msg = string;

  return NO_ERROR;
}



/**
 * create_msg_get_folder_meta_request
 *
 * Generates a message string requesting the list of files, folders, and
 * associated metadata within the specified folder; Returns NO_ERROR or an error
 * code
 *
 * dirname  - null-terminated string containing the fully-qualified path of the
 *            target directory
 * msg      - (return val) pointer where the message string will be stored
 * msg_len  - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
uint8_t create_msg_get_folder_meta_request(char *dirname, char **msg, 
                                           uint64_t *msg_len)
{
  return msg_with_single_element(GET, BACS_FOLDER, BACS_REQUEST, dirname, 
    (uint32_t)strlen(dirname), msg, msg_len);
}



/**
 * create_msg_get_folder_meta_response
 *
 * Generates a message response string include the list basic_meta_t's for both
 * the files and folders contained in the specified meta_t; returns NO_ERROR or
 * an error code
 *
 * folder   - meta_t for the folder containing the metadata to send
 * msg      - (return val) pointer where the message string will be stored
 * msg_len  - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
uint8_t create_msg_get_folder_meta_response(meta_t *folder, char **msg, 
                                            uint64_t *msg_len)
{
  char *string;
  int index, i, num_metas;
  meta_t *current;

  /* Calculate the number of bytes in the message */
  uint32_t num_chars = BACS_HEADER_SIZE +
    sizeof(uint32_t);  /* Number of basic_meta_t's in the message */

  /* Add the bytes needed for the subfolder and file metadata */
  num_metas = 0;
  for(i=0; i<2; i++) {
    current = (i == 0) ? folder->subfolders : folder->files;

    while(current != NULL) {
      /* Only count READY and DOWNLOADING files */
      if(current->status == READY || current->status == DOWNLOADING) {
        num_chars = num_chars +
          sizeof(uint8_t) +       /* Meta type information */
          sizeof(uint16_t) +      /* File/folder name length */
          strlen(current->name) + /* File/folder name */
          sizeof(uint64_t);       /* Bytes contained in file/folder */

        num_metas++;
      }

      current = current->next;
    }
  }

  /* Allocate memory for the message */
  string = calloc(num_chars, sizeof(char));
  if(string == NULL) return ERR_MEM_ALLOC;
  memset(string, 0, num_chars*sizeof(char));

  /* Slap a header on the message */
  generate_header(GET, BACS_FOLDER, BACS_RESPONSE, string);

  /* Add the count of basic_meta_t's to the message */
  index = BACS_HEADER_SIZE;
  *(uint32_t *)&string[index] = num_metas;
  index = index + sizeof(uint32_t);

  /* Add each subfolder's and file's metadata to the message */
  for(i=0; i<2; i++) {
    current = (i == 0) ? folder->subfolders : folder->files;

    while(current != NULL) {
      /* Only add READY and DOWNLOADING files */
      if(current->status == READY || current->status == DOWNLOADING) {
        /* Add the meta_t's type */
        *(uint8_t *)&string[index] = current->type;
        index = index + sizeof(uint8_t);

        /* Add the meta_t's name */
        *(uint16_t *)&string[index] = strlen(current->name);
        index = index + sizeof(uint16_t);
        strncpy(&string[index], current->name, strlen(current->name));
        index = index + strlen(current->name);

        /* Add the meta_t's size */
        *(uint64_t *)&string[index] = current->size;
        index = index + sizeof(uint64_t);
      }

      /* Advance to next meta */
      current = current->next;
    }
  }

  /* Set the return values */
  *msg_len = num_chars;
  *msg = string;

  return NO_ERROR;
}



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
void create_msg_get_servers_request(char **msg, uint64_t *msg_len)
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
void create_msg_get_servers_response(char **msg, uint64_t *msg_len)
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
                                   char **msg, uint64_t *msg_len)
{
  msg_with_block(POST, BACS_BLOCK, BACS_REQUEST, uuid, size, content, 
                 msg, msg_len);
}



/**
 * create_msg_post_block_response
 *
 * Generates a message response string signifying that a block was successfully 
 * uploaded
 *
 * uuid     - UUID of the block that was successfully uploaded
 * msg      - (return val) pointer where the message string will be stored
 * msg_len  - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
void create_msg_post_block_response(uuid_t uuid, char **msg, uint64_t *msg_len)
{
  msg_with_single_element(POST, BACS_BLOCK, BACS_RESPONSE, 
                          uuid, sizeof(uuid_t), msg, msg_len);
}



/**
 * create_msg_post_file_request
 *
 * Generates a message string submitting the metadata for a new file
 *
 * filename  - null-terminated string containing the filename of the new file
 * file_size - number of bytes in the new file
 * msg       - (return val) pointer where the message string will be stored
 * msg_len   - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
void create_msg_post_file_request(char *filename, uint64_t file_size, 
                                  char **msg, uint64_t *msg_len)
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
 * Generates a response string containing a list of block IDs in the file
 *
 * msg      - (return val) pointer where the message string will be stored
 * msg_len  - (return val) pointer to size of 'msg' string
 * 
 * NOTE: the method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
void create_msg_post_file_response(meta_t *file, char **msg, uint64_t *msg_len)
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
 * create_msg_post_folder_request
 *
 * Generates a message string submitting the name for a new folder
 *
 * foldername - null-terminated string containing the path of the new folder
 * msg        - (return val) pointer where the message string will be stored
 * msg_len    - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
void create_msg_post_folder_request(char *foldername, char **msg, 
                                    uint64_t *msg_len)
{
  msg_with_single_element(POST, BACS_FOLDER, BACS_REQUEST, foldername, 
                          (uint32_t)strlen(foldername), msg, msg_len);
}



/**
 * create_msg_post_folder_response
 *
 * Generates a message response string signifying that a folder was successfully 
 * created
 *
 * msg      - (return val) pointer where the message string will be stored
 * msg_len  - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
void create_msg_post_folder_response(char **msg, uint64_t *msg_len)
{
  msg_with_single_element(POST, BACS_FOLDER, BACS_RESPONSE, 
                          NULL, 0, msg, msg_len);
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
 * msg_with_block
 *
 * Populates a message string with the specified headers and a block; Returns
 * NO_ERROR or an error code
 *
 * action   - message action identifier
 * resource - target resource for this message
 * type     - type of message
 * uuid     - UUID of the block being uploaded
 * size     - number of bytes in the block
 * content  - raw content of the block
 * msg      - (return val) pointer where the message string will be stored
 * msg_len  - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
uint8_t msg_with_block(uint8_t action, uint8_t resource, uint8_t type,
                       uuid_t uuid, uint32_t size, char *content,
                       char **msg, uint64_t *msg_len)
{
  char *string;
  uint32_t index;
  
  /* Calculate the number of bytes in this message */
  *msg_len = BACS_HEADER_SIZE +
    sizeof(uuid_t) +    /* length of UUID */
    sizeof(uint32_t) +  /* block size */
    /*sizeof(uint32_t) +*/  /* block checksum */
    size;               /* size of block */

  /* Allocate memory for the message string and initialize it */
  string = calloc(*msg_len, sizeof(char));
  if(string == NULL) return ERR_MEM_ALLOC;
  memset(string, 0, *msg_len * sizeof(char));

  /* Slap a header on the message */
  generate_header(action, resource, type, string);

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

  return NO_ERROR;
}



/**
 * msg_with_single_element
 *
 * Populates a message string with the specified headers and single data element;
 * Returns either NO_ERROR or an error code
 *
 * action      - message action identifier
 * resource    - target resource for this message
 * type        - type of message
 * element     - string containing the data element
 * element_len - number of bytes the element occupies
 * msg         - (return val) pointer where the message string will be stored
 * msg_len     - (return val) pointer to size of 'msg' string
 * 
 * NOTE: this method allocates memory for 'msg'; it is the responsiblity of the 
 *       caller to free the allocation
 */
uint8_t msg_with_single_element(uint8_t action, uint8_t resource, uint8_t type, 
                                void *element, uint32_t element_len, char **msg, 
                                uint64_t *msg_len)
{
  char *string;
  int index;

  /* Calculate the number of bytes in this message */
  uint32_t num_chars = BACS_HEADER_SIZE +
    sizeof(uint32_t) +  /* Element length */
    element_len;        /* Element content */

  /* Allocate memory for the message string */
  string = calloc(num_chars, sizeof(char));
  if(string == NULL) 
    return ERR_MEM_ALLOC;
  memset(string, 0, num_chars*sizeof(char));

  /* Slap a header on the message */
  generate_header(action, resource, type, string);

  /* Add the element to the message after the header */
  index = BACS_HEADER_SIZE;
  *(uint32_t *)&string[index] = element_len;
  index = index + sizeof(uint32_t);
  if(element_len > 0) memcpy(&string[index], element, element_len);

  /* Set the return values */
  *msg_len = num_chars;
  *msg = string;

  return NO_ERROR;
}



/**
 * parse_msg_block
 *
 * Extracts the UUID, content size, and content of the block from the request
 *
 * action   - message action identifier
 * resource - target resource for this message
 * type     - type of message
 * msg      - the message to parse
 * uuid     - (return val) pointer to uuid_t where UUID should be written
 * size     - (return val) pointer to number of bytes in the block
 * content  - (return val) pointer to a string to contain the block's content
 * 
 * NOTE: this method allocates memory for 'content'; it is the responsiblity 
 *       of the caller to free the allocation
 */
void parse_msg_block(char *msg, uint8_t action, uint8_t resource, uint8_t type, 
                     uuid_t *uuid, uint32_t *size, char **content)
{
  uint32_t index;
  uuid_t *msg_uuid;
  char *string;

  /* Do a sanity check to make sure we got the expected type of message */
  if(get_header_resource(msg) != resource || 
     get_header_action(msg) != action ||
     get_header_type(msg) != type)
    die_with_error("parse_msg_block - invalid message header");

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
    die_with_error("parse_msg_block - calloc failed");
  memset(string, 0, *size * sizeof(char));

  /* Extract content from message */
  index = index + sizeof(uint32_t);
  strncpy(string, &msg[index], *size);
  *content = string;

  /* TODO: Extract checksum from message */
}



/**
 * parse_msg_error
 *
 * Extracts the error message from the message; Returns NO_ERROR or error code
 *
 * msg     - the message to parse
 * err_msg - (return val) pointer to a string where the error message should be
 *           stored
 * 
 * NOTE: this method allocates memory for 'err_msg'; it is the responsiblity 
 *       of the caller to free the allocation
 */
uint8_t parse_msg_error(char *msg, char **err_msg)
{
  return parse_msg_single_string(msg, get_header_action(msg), 
    get_header_resource(msg), BACS_ERROR, err_msg);
}



/**
 * parse_msg_get_block_request
 *
 * Extracts the UUID of the block from the request; Returns NO_ERROR or an error
 * code
 *
 * msg     - the message to parse
 * uuid    - (return val) pointer to uuid_t where UUID should be written
 */
uint8_t parse_msg_get_block_request(char *msg, uuid_t *uuid)
{
  char *str_uuid;
  uuid_t *msg_uuid;
  uint8_t err_code;

  /* Get UUID encoded as a string from message */
  err_code = parse_msg_single_string(msg, GET, BACS_BLOCK, BACS_REQUEST, 
                                     &str_uuid);
  if(err_code != NO_ERROR) return err_code;

  /* Extract UUID from string */
  msg_uuid = (uuid_t *)str_uuid;
  uuid_copy(*uuid, *msg_uuid);

  /* Clean up */
  free(str_uuid);

  return NO_ERROR;
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
void parse_msg_get_block_response(char *msg, uuid_t *uuid, uint32_t *size, 
                                  char **content)
{
  parse_msg_block(msg, GET, BACS_BLOCK, BACS_RESPONSE, uuid, size, content);
}



/**
 * parse_msg_get_file_request
 *
 * Extracts the filename from the request; Returns NO_ERROR or an error code
 *
 * msg      - the message to parse
 * filename - (return val) pointer to string where filename should be stored
 * 
 * NOTE: this method allocates memory for 'filename'; it is the responsiblity 
 *       of the caller to free the allocation
 */
uint8_t parse_msg_get_file_request(char *msg, char **filename)
{
  return parse_msg_single_string(msg, GET, BACS_FILE, BACS_REQUEST, filename);
}



/**
 * parse_msg_get_file_response
 *
 * Extracts the basic_block_t's from the response
 *
 * msg        - the message to parse
 * blocks     - (return val) pointer to destination array of basic_block_t's
 * num_blocks - (return val) pointer to count of basic_block_t's in array
 * 
 * NOTE: this method allocates memory for 'blocks' and the 'host_ips' within 
 *       each basic_block_t; it is the responsiblity of the caller to free the 
 *       memory allocation
 */
void parse_msg_get_file_response(char *msg, basic_block_t **blocks, 
                                 uint64_t *num_blocks)
{
  basic_block_t *basic_blocks;
  /* uint16_t string_len; */
  uint64_t index, i;

  /* Do a sanity check to make sure we got the expected type of message */
  if(get_header_resource(msg) != BACS_FILE || 
     get_header_action(msg) != GET ||
     get_header_type(msg) != BACS_RESPONSE)
    die_with_error("parse_msg_get_file_response - invalid message header");

  /* Extract the number of basic_block_t's from the message */
  index = BACS_HEADER_SIZE;
  *num_blocks = *(uint64_t *)&msg[index];

  /* Allocate memory for the basic_block_t's */
  basic_blocks = calloc(*num_blocks, sizeof(basic_block_t));
  if(basic_blocks == NULL) 
    die_with_error("parse_msg_get_file_response - calloc failed");
  memset(basic_blocks, 0, *num_blocks * sizeof(basic_block_t));

  /* Extract each basic_block_t from message */
  index = index + sizeof(uint64_t);
  for(i=0; i < *num_blocks; i++) {
    
    /* Extract block UUID */
    uuid_copy(basic_blocks[i].uuid, *(uuid_t *)&msg[index]);
    index = index + sizeof(uuid_t);

    /* TODO: Extract checksum */

    /* Extract number of hosts */
    basic_blocks[i].num_hosts = *(uint8_t *)&msg[index];
    index = index + sizeof(uint8_t);

    /* TODO: Extract host IPs */
  }

  *blocks = basic_blocks;
}



/**
 * parse_msg_get_folder_meta_request
 *
 * Extracts the folder name from the request; returns NO_ERROR or an error code
 *
 * msg      - the message to parse
 * dirname  - (return val) pointer to string where folder name should be stored
 * 
 * NOTE: this method allocates memory for 'dirname'; it is the responsiblity 
 *       of the caller to free the allocation
 */
uint8_t parse_msg_get_folder_meta_request(char *msg, char **dirname)
{
  return parse_msg_single_string(msg, GET, BACS_FOLDER, BACS_REQUEST, dirname);
}



/**
 * parse_msg_get_folder_meta_response
 *
 * Extracts the basic_meta_t's from the response
 *
 * msg       - the message to parse
 * metas     - (return val) pointer to destination array of basic_meta_t's
 * num_metas - (return val) pointer to count of basic_meta_t's in array
 * 
 * NOTE: this method allocates memory for 'metas' and the 'name' within each
 *       basic_meta_t; it is the responsiblity of the caller to free the 
 *       memory allocation
 */
void parse_msg_get_folder_meta_response(char *msg, basic_meta_t **metas, 
                                        uint32_t *num_metas)
{
  basic_meta_t *basic_metas;
  uint16_t string_len;
  uint32_t index, i;

  /* Do a sanity check to make sure we got the expected type of message */
  if(get_header_resource(msg) != BACS_FOLDER || 
     get_header_action(msg) != GET ||
     get_header_type(msg) != BACS_RESPONSE)
    die_with_error("parse_msg_get_folder_meta_response - invalid message header");

  /* Extract the number of metas from the message */
  index = BACS_HEADER_SIZE;
  *num_metas = *(uint32_t *)&msg[BACS_HEADER_SIZE];

  /* Allocate memory for the basic_meta_t's */
  basic_metas = calloc(*num_metas, sizeof(basic_meta_t));
  if(basic_metas == NULL) 
    die_with_error("parse_msg_get_folder_meta_response - calloc failed");
  memset(basic_metas, 0, *num_metas * sizeof(basic_meta_t));

  /* Extract each basic_meta_t from message */
  index = index + sizeof(uint32_t);
  for(i=0; i < *num_metas; i++) {
    /* Extract meta type */
    basic_metas[i].type = *(uint8_t *)&msg[index];
    index = index + sizeof(uint8_t);

    /* Extract meta name */
    string_len = *(uint16_t *)&msg[index];
    index = index + sizeof(uint16_t);
    basic_metas[i].name = calloc(string_len + 1, sizeof(char));
    memset(basic_metas[i].name, 0, (string_len + 1) * sizeof(char));
    strncpy(basic_metas[i].name, &msg[index], string_len);
    index = index + string_len;

    /* Extract meta size */
    basic_metas[i].size = *(uint64_t *)&msg[index];
    index = index + sizeof(uint64_t);
  }

  *metas = basic_metas;
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
  parse_msg_block(msg, POST, BACS_BLOCK, BACS_REQUEST, uuid, size, content);
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
  char *str_uuid;
  uuid_t *msg_uuid;

  /* Get UUID encoded as a string from message */
  parse_msg_single_string(msg, POST, BACS_BLOCK, BACS_RESPONSE, &str_uuid);

  /* Extract UUID from string */
  msg_uuid = (uuid_t *)str_uuid;
  uuid_copy(*uuid, *msg_uuid);

  /* Clean up */
  free(str_uuid);
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
 * parse_msg_post_folder_request
 *
 * Extracts the name of the new folder from the request; Returns NO_ERROR or an
 * error code
 *
 * msg        - the message to parse
 * foldername - (return val) pointer to a string where the new folder's name 
 *              should be stored
 * 
 * NOTE: this method allocates memory for 'foldername'; it is the responsiblity 
 *       of the caller to free the allocation
 */
uint8_t parse_msg_post_folder_request(char *msg, char **foldername)
{
  return parse_msg_single_string(msg, POST, BACS_FOLDER, BACS_REQUEST, 
                                 foldername);
}



/**
 * parse_msg_post_folder_response
 *
 * Checks the headers from the response
 *
 * msg     - the message to parse
 */
void parse_msg_post_folder_response(char *msg)
{
  parse_msg_single_string(msg, POST, BACS_FOLDER, BACS_RESPONSE, NULL);
}



/**
 * parse_msg_single_string
 *
 * Parses a message string with the specified headers containing a single string;
 * Returns NO_ERROR or an error code
 *
 * msg      - the message to parse
 * action   - message action identifier
 * resource - target resource for this message
 * type     - type of message
 * str      - (ret value) pointer to string to store message's single string
 * 
 * NOTE: this method allocates memory for 'str'; it is the responsiblity of the 
 *       caller to free the allocation
 */
uint8_t parse_msg_single_string(char *msg, uint8_t action, uint8_t resource, 
                                uint8_t type, char **str)
{
  char *string;
  uint32_t string_len;
  uint32_t index;

  /* Do a sanity check to make sure we got the expected type of message */
  if(get_header_resource(msg) != resource || 
     get_header_action(msg) != action ||
     get_header_type(msg) != type)
    die_with_error("parse_msg_single_string - invalid message header");

  if(str != NULL) {
    /* Extract the length of the string from the message */
    index = BACS_HEADER_SIZE;
    string_len = *(uint32_t *)&msg[BACS_HEADER_SIZE];

    /* Allocate memory for the string; Add one more char to the end for a 
     * null terminator */
    string = calloc(string_len + 1, sizeof(char));
    if(string == NULL) return ERR_MEM_ALLOC;
    memset(string, 0, (string_len + 1) * sizeof(char));

    /* Extract string from message */
    index = index + sizeof(uint32_t);
    strncpy(string, &msg[index], string_len);
    *str = string;
  }

  return NO_ERROR;
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
            case GET:   print_msg_get_block_request(msg); break;
            case POST:  print_msg_post_block_request(msg); break;
            default:    printf("INVALID ACTION");
          }

          break;

        /* FILE requests */
        case BACS_FILE:  
          switch(get_header_action(msg)) {
            case GET:   print_msg_get_file_request(msg); break;
            case POST:  print_msg_post_file_request(msg); break;
            default:    printf("INVALID ACTION");
          }

          break;

        /* FOLDER requests */
        case BACS_FOLDER:  
          switch(get_header_action(msg)) {
            case GET:  print_msg_get_folder_meta_request(msg); break;
            case POST: print_msg_post_folder_request(msg); break;
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
            case GET:   print_msg_get_block_response(msg); break;
            case POST:  print_msg_post_block_response(msg); break;
            default:    printf("INVALID ACTION");
          }

          break;

        /* FILE responses */
        case BACS_FILE:  
          switch(get_header_action(msg)) {
            case GET:   print_msg_get_file_response(msg); break;
            case POST:  print_msg_post_file_response(msg); break;
            default:    printf("INVALID ACTION");
          }

          break;

        /* FOLDER responses */
        case BACS_FOLDER:  
          switch(get_header_action(msg)) {
            case GET:  print_msg_get_folder_meta_response(msg); break;
            case POST: print_msg_post_folder_response(msg); break;
            default:    printf("INVALID ACTION");
          }

          break;


        default: printf("UNKNOWN RESOURCE");
      }

      break;

    case BACS_ERROR: print_msg_error(msg); break;  

    default: printf("UNKNOWN MESSAGE TYPE");
  }

  printf("\n");
}



/**
 * print_msg_error
 *
 * DEBUGGING; Prints out contents of error messages in human-readable format
 */
void print_msg_error(char *msg)
{
  char *err_msg;
  parse_msg_error(msg, &err_msg);
  printf("%s\n", err_msg);
  free(err_msg);
}



/**
 * print_msg_get_block_request
 *
 * DEBUGGING; Prints out contents of GET BACS_BLOCK request in human-readable 
 * format
 */
void print_msg_get_block_request(char *msg)
{
  uuid_t uuid;
  char *str;

  parse_msg_get_block_request(msg, &uuid);
  str = uuid_str(uuid);
  printf("%s\n", str);
  free(str);
}



/**
 * print_msg_get_block_response
 *
 * DEBUGGING; Prints out contents of GET BACS_BLOCK response in human-readable 
 * format
 */
void print_msg_get_block_response(char *msg)
{
  uuid_t uuid;
  uint32_t block_size;
  char *content, *str;

  parse_msg_get_block_response(msg, &uuid, &block_size, &content);
  str = uuid_str(uuid);
  printf("%s\n", str);
  free(str);
  printf(" - Content (%d bytes): %s\n", block_size, content);
  free(content);

  /* TODO: Add checksum to message */
}



/**
 * print_msg_get_file_request
 *
 * DEBUGGING; Prints out contents of GET FILE request in human-readable 
 * format
 */
void print_msg_get_file_request(char *msg)
{
  char *filename;
  parse_msg_get_file_request(msg, &filename);
  printf("%s\n", filename);
  free(filename);
}



/**
 * print_msg_get_file_response
 *
 * DEBUGGING; Prints out contents of GET FILE response in human-readable 
 * format
 */
void print_msg_get_file_response(char *msg)
{
  uint64_t num_blocks, i;
  basic_block_t *blocks;
  parse_msg_get_file_response(msg, &blocks, &num_blocks);
  printf("%" PRIu64 " basic_block_t's:", num_blocks);
  for(i=0; i < num_blocks; i++) {
    char *str = uuid_str(blocks[i].uuid);
    printf("\n - %s - Hosts: %d", str, blocks[i].num_hosts);
    free(str);
  }
  free(blocks);
  printf("\n");
}



/**
 * print_msg_get_folder_meta_request
 *
 * DEBUGGING; Prints out contents of GET FOLDER_META request in human-readable 
 * format
 */
void print_msg_get_folder_meta_request(char *msg)
{
  char *dirname;
  parse_msg_get_folder_meta_request(msg, &dirname);
  printf("%s\n", dirname);
  free(dirname);
}



/**
 * print_msg_get_folder_meta_response
 *
 * DEBUGGING; Prints out contents of GET FOLDER_META response in human-readable 
 * format
 */
void print_msg_get_folder_meta_response(char *msg)
{
  uint32_t num_metas, i;
  basic_meta_t *metas;
  parse_msg_get_folder_meta_response(msg, &metas, &num_metas);
  printf("%d basic_meta_t's:", num_metas);
  for(i=0; i < num_metas; i++) {
    printf("\n - %s: '%s' (%" PRIu64 " bytes)", 
      meta_type_string(metas[i].type), metas[i].name, metas[i].size);
      free(metas[i].name);
  }
  free(metas);
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



/**
 * print_msg_post_folder_request
 *
 * DEBUGGING; Prints out contents of POST BACS_FOLDER request in human-readable 
 * format
 */
void print_msg_post_folder_request(char *msg)
{
  char *foldername;
  parse_msg_post_folder_request(msg, &foldername);
  printf("%s", foldername);
  free(foldername);
}



/**
 * print_msg_post_folder_response
 *
 * DEBUGGING; Prints out contents of POST BACS_FOLDER response in human-readable 
 * format
 */
void print_msg_post_folder_response(char *msg)
{
  parse_msg_post_folder_response(msg);
  printf("SUCCESS");
}
