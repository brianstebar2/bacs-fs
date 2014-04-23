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

#include "blocks.h"
#include "common.h"
#include "file_metadata.h"
#include "messages.h"
#include "server_functions.h"



/**
 * handle_get_folder_meta
 * 
 * Returns the metadata of all files and subfolders within the target folder
 *
 * NOTE: this method allocates memory for 'response'; it is the responsiblity 
 *       of the caller to free the allocation
 */
void handle_get_folder_meta(char *msg, char **response, uint64_t *response_len)
{
  char *dirname;
  meta_t *folder_meta;

  /* TODO: Check message parse for errors */
  parse_msg_get_folder_meta_request(msg, &dirname);

  /* Lookup target folder */
  /* TODO: Check file_meta for error return code */
  folder_meta = find_meta(fs_metadata, dirname, BACS_FOLDER);

  /* Create response containing metadata for folder */
  /* TODO: Check message creation for errors */
  create_msg_get_folder_meta_response(folder_meta, response, response_len);

  /* Clean up local data */
  free(dirname);
}



/**
 * handle_post_block
 * 
 * Populates a block with the content in the message
 *
 * NOTE: this method allocates memory for 'response'; it is the responsiblity 
 *       of the caller to free the allocation
 */
void handle_post_block(char *msg, char **response, uint64_t *response_len)
{
  uint32_t block_size;
  uint64_t i;
  char *block_content, *uuid_string;
  uuid_t block_uuid;
  block_t *block_ptr;
  meta_t *file_meta;

  /* TODO: Check message parse for errors */
  parse_msg_post_block_request(msg, &block_uuid, &block_size, &block_content);

  /* Lookup block to populate */
  /* TODO: Check lookup for errors */
  uuid_string = uuid_str(block_uuid);
  block_ptr = find_block(block_uuid);
  printf("Populating block %s in file '%s'\n", 
    uuid_string, block_ptr->parent->name);

  /* Populate block content */
  /* TODO: Check populate for errors */
  populate_block(block_ptr, block_content, block_size);
  
  /* Scan through the blocks and see if they're all ready now */
  file_meta = block_ptr->parent;
  for(i=0; i < file_meta->num_blocks; i++)
    if(file_meta->blocks[i]->status != READY) break;

  /* If the scan got through all the blocks, we can mark the file as ready */
  if(i == file_meta->num_blocks) file_meta->status = READY;

  /* Create response indicating success */
  /* TODO: Check message creation for errors */
  create_msg_post_block_response(block_uuid, response, response_len);

  /* Clean up local data */
  free(uuid_string);
  free(block_content);
}



/**
 * handle_post_file
 * 
 * Adds a file entry to the file system metadata
 *
 * NOTE: this method allocates memory for 'response'; it is the responsiblity 
 *       of the caller to free the allocation
 */
void handle_post_file(char *msg, char **response, uint64_t *response_len)
{
  uint64_t file_size;
  char *filename;
  meta_t *file_meta;

  /* TODO: Check message parse for errors */
  parse_msg_post_file_request(msg, &filename, &file_size);
  
  /* Add metadata for new file */
  /* TODO: Check file_meta for error return code */
  printf("Creating metadata for file '%s'\n", filename);
  file_meta = add_file_meta(fs_metadata, filename, file_size, 0);

  /* Create response containing list of UUIDs for new file's blocks */
  /* TODO: Check message creation for errors */
  create_msg_post_file_response(file_meta, response, response_len);

  /* Clean up local data */
  free(filename);
}



/**
 * handle_post_folder
 * 
 * Adds a folder entry to the file system metadata
 *
 * NOTE: this method allocates memory for 'response'; it is the responsiblity 
 *       of the caller to free the allocation
 */
void handle_post_folder(char *msg, char **response, uint64_t *response_len)
{
  char *dirname;
  meta_t *file_meta;

  /* TODO: Check message parse for errors */
  parse_msg_post_folder_request(msg, &dirname);

  /* Create the new folder */
  /* TODO: Check file_meta for error return code */
  printf("Creating folder '%s'\n", dirname);
  file_meta = add_folder(fs_metadata, dirname);

  /* Create response indicicating success */
  /* TODO: Check message creation for errors */
  create_msg_post_folder_response(response, response_len);

  /* Clean up local data */
  free(dirname);
}



/**
 * start_listening
 * 
 * Listens for and responds to incoming requests
 */
void start_listening()
{
  char *msg, *resp; 
  uint64_t len, resp_len;




  /* Debugging crap... */
  uuid_t *uuids;
  uint64_t num_uuids, i;
  meta_t *file_meta;

  /* Client: Send a request to upload a new file */
  create_msg_post_file_request("/awesome/bad/c.txt", 4096, &msg, &len);  
  print_msg(msg);
  handle_post_file(msg, &resp, &resp_len);
  print_msg(resp);
  free(msg);

  printf("SERVER META TREE\n");
  print_meta_tree(fs_metadata, "");
  printf("\n");
  
  /* Client: Take list of UUIDs and send each block */
  parse_msg_post_file_response(resp, &uuids, &num_uuids);
  free(resp);
  for(i=0; i < num_uuids; i++) {
    block_t *block_ptr = find_block(uuids[i]);
    
    /* Client: send a block */
    char block[DEFAULT_BLOCK_SIZE] = {0};
    sprintf(block, "Block #%" PRIu64 " content", i);
    /* TODO: Add checksum to post block request message */
    create_msg_post_block_request(uuids[i], DEFAULT_BLOCK_SIZE, block, &msg, &len);
    print_msg(msg);

    /* Server: write block and update block metadata */
    handle_post_block(msg, &resp, &resp_len);
    print_msg(resp);
    free(msg);
    free(resp);

    printf("SERVER FILE META\n");
    print_file_meta(block_ptr->parent);
  }

  /* Client: create a new folder */
  create_msg_post_folder_request("/awesome/cool", &msg, &len);
  print_msg(msg);

  /* Server: create the new folder */
  handle_post_folder(msg, &resp, &resp_len);
  print_msg(resp);
  free(msg);
  free(resp);

  printf("\nSERVER META TREE\n");
  print_meta_tree(fs_metadata, "");
  printf("\n");

  /* Add another file to the file system */
  file_meta = add_file_meta(fs_metadata, "/awesome/d.txt", 8000, 0);
  printf("Added /awesome/d.txt; UUIDs returned: %" PRIu64 "\n", file_meta->num_blocks);

  printf("\nSERVER META TREE\n");
  print_meta_tree(fs_metadata, "");
  printf("\n");

  /* Client: Send a request to the server for folder metadata */
  create_msg_get_folder_meta_request("/awesome", &msg, &len);
  // printf("Client: Sending message (%d bytes):\n", len);
  // print_msg(msg);

  // /* Server: send response containing metadata for folder */
  // parse_msg_get_folder_meta_request(msg, &dirname);
  // free(msg);
  // file_meta = find_meta(fs_metadata, dirname, BACS_FOLDER);
  // free(dirname);
  // create_msg_get_folder_meta_response(file_meta, &msg, &len);
  // printf("Server: Sending message (%d bytes):\n", len);
  // print_msg(msg);
  // free(msg);







  /* Handle incoming messages */
  /*while(msg = myrecv()) {*/
  while(1) {
    switch(get_header_type(msg)) {
      case BACS_REQUEST: 
        switch(get_header_resource(msg)) {
          
          /* BLOCK requests */
          case BACS_BLOCK:  
            switch(get_header_action(msg)) {
              case POST:  handle_post_block(msg, &resp, &resp_len); break;
              default:    printf("Invalid message action; send error message.\n");
            }

            break;

          /* FILE requests */
          case BACS_FILE:  
            switch(get_header_action(msg)) {
              case POST:  handle_post_file(msg, &resp, &resp_len); break;
              default:    printf("Invalid message action; send error message.\n");
            }
            break;

          /* FOLDER requests */
          case BACS_FOLDER:  
            switch(get_header_action(msg)) {
              case GET:   handle_get_folder_meta(msg, &resp, &resp_len); break;
              case POST:  handle_post_folder(msg, &resp, &resp_len); break;
              default:    printf("Invalid message action; send error message.\n");
            }
            break;

          default: printf("Unknown message resource; send error message.\n");
        }
        break;

      default: printf("Unknown message type; send error message.\n");
    }


    /* Send response if necessary */
    if(resp_len > 0) {
      /* mysend(msg_src_ip, response, response_len); */
      print_msg(resp);
    }

    /* Clean up request and response */    
    free(msg);
    free(resp);
    msg = NULL;
    resp = NULL;
  }
}
