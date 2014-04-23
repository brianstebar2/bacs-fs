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
#include <sys/stat.h>
#include <sys/types.h>

#include "blocks.h"
#include "file_metadata.h"
#include "messages.h"
#include "server_functions.h"

/* Debugging includes */
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <uuid/uuid.h>
#include "common.h"




int main(int argc, char **argv)
{
  /*uint64_t file_size, num_uuids, num_blocks;
  char *msg, *dirname, *filename;
  uint32_t len, i;
  meta_t *file_meta;
  uuid_t *uuids;
  basic_block_t *basic_blocks;*/

  printf("BACS - Starting file server...\n");

  /* Initialize the file metadata tree */
  fs_metadata = create_meta_t();
  fs_metadata->status = READY;

  /* Initialize server block storage */
  all_blocks = NULL;
  all_blocks_num = 0;
  block_data_path = DEFAULT_DATA_PATH;

  /* If data folder for blocks doesn't exist, create it */
  mkdir(block_data_path, S_IRWXU | S_IRWXG | S_IRWXO);

  /* TODO: read in blocks already stored */

  /* TODO: retrieve metadata from master */



  /* THIS IS WHERE WE SHOULD LISTEN FOR MESSAGES */
  printf("BACS - Server ready.\n");
  start_listening();



  // /* Debugging crap... */
  // /* Client: Send a request to upload a new file */
  // create_msg_post_file_request("/awesome/bad/c.txt", 4096, &msg, &len);  
  // printf("Client: Sending message (%d bytes): \n", len);
  // print_msg(msg);
  // printf("\n");

  // /* Server: Add metadata for new file */
  // parse_msg_post_file_request(msg, &filename, &file_size);
  // free(msg);
  // file_meta = add_file_meta(fs_metadata, filename, file_size, 0);
  // printf("SERVER META TREE\n");
  // print_meta_tree(fs_metadata, "");
  // printf("\n");
  // free(filename);

  // /* Server: send response containing list of UUIDs for new file's blocks */
  // create_msg_post_file_response(file_meta, &msg, &len);
  // printf("Server: Sending message (%d bytes): \n", len);
  // print_msg(msg);
  
  // /* Client: Take list of UUIDs and send each block */
  // parse_msg_post_file_response(msg, &uuids, &num_uuids);
  // free(msg);
  // for(i=0; i < num_uuids; i++) {
  //   uint32_t block_size;
  //   char *block_content;
  //   uuid_t block_uuid;
  //   char block[DEFAULT_BLOCK_SIZE] = {0};
  //   block_t *block_ptr;
    
  //   /* Client: send a block */
  //   sprintf(block, "Block #%d content", i);
  //   /* TODO: Add checksum to post block request message */
  //   create_msg_post_block_request(uuids[i], DEFAULT_BLOCK_SIZE, block, &msg, &len);
  //   printf("Client: Sending message: (%d bytes): \n", len);
  //   print_msg(msg);

  //   /* Server: write block and update block metadata */
  //   parse_msg_post_block_request(msg, &block_uuid, &block_size, &block_content);
  //   free(msg);

  //   /* Populate block content */
  //   block_ptr = find_block(block_uuid);
  //   populate_block(block_ptr, block_content, block_size);
  //   /* TODO: Server updates file status if all blocks in the file are ready */

  //   /*printf("SERVER FILE META\n");
  //   print_file_meta(file_meta);*/

  //   /* Server: send response indicicating success */
  //   create_msg_post_block_response(uuids[i], &msg, &len);
  //   printf("Server: Sending message (%d bytes):\n", len);
  //   print_msg(msg);
  //   free(msg);
  // }

  // /* Client: create a new folder */
  // create_msg_post_folder_request("/awesome/cool", &msg, &len);
  // printf("Client: Sending message: (%d bytes): \n", len);
  // print_msg(msg);

  // /* Server: create the new folder */
  // parse_msg_post_folder_request(msg, &dirname);
  // free(msg);
  // file_meta = add_folder(fs_metadata, dirname);
  // printf("\nSERVER META TREE\n");
  // print_meta_tree(fs_metadata, "");
  // printf("\n");

  // /* Server: send response indicicating success */
  // create_msg_post_folder_response(&msg, &len);
  // printf("Server: Sending message (%d bytes):\n", len);
  // print_msg(msg);
  // free(msg);

  // file_meta = add_file_meta(fs_metadata, "/awesome/d.txt", 8000, 0);
  // printf("Added /awesome/d.txt; UUIDs returned: %" PRIu64 "\n", file_meta->num_blocks);
  // printf("[ ");
  // for(i = 0; i < file_meta->num_blocks; i++) {
  //   printf("%s ", uuid_str(file_meta->blocks[i]->uuid));
  // }
  // printf("]\n");

  // printf("\nSERVER META TREE\n");
  // print_meta_tree(fs_metadata, "");
  // printf("\n");

  // /* Have a look at the directory /awesome contents */
  // /* Client: Send a request to the server for folder metadata */
  // create_msg_get_folder_meta_request("/awesome", &msg, &len);
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
  
  // /* Have a look at the directory /awesome/bad contents */
  // /* Client: Send a request to the server for folder metadata */
  // create_msg_get_folder_meta_request("/awesome/bad", &msg, &len);
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


  // /* Client: Send a message requesting to download a file */
  // create_msg_get_file_request("/awesome/bad/c.txt", &msg, &len);
  // printf("Client: Sending message (%d bytes):\n", len);
  // print_msg(msg);

  // /* Server: send response containing UUIDs and IP addresses for file's blocks */
  // parse_msg_get_file_request(msg, &filename);
  // free(msg);
  // file_meta = find_meta(fs_metadata, filename, BACS_FILE);
  // create_msg_get_file_response(file_meta, &msg, &len);
  // printf("Server: Sending message (%d bytes):\n", len);
  // print_msg(msg);
  

  // /* Iteratively download each block */
  // parse_msg_get_file_response(msg, &basic_blocks, &num_blocks);
  // free(msg);

  // for(i=0; i < num_blocks; i++) {
  //   uuid_t uuid;
  //   block_t *block_ptr;
  //   char *block_content;

  //   /* Client: Take each basic_block_t and request the actual blocks */
  //   create_msg_get_block_request(basic_blocks[i].uuid, &msg, &len);
  //   printf("Client: Sending message (%d bytes):\n", len);
  //   print_msg(msg);

  //   /* Server: look up the block and retrieve its content */
  //   parse_msg_get_block_request(msg, &uuid);
  //   free(msg);
  //   block_ptr = find_block(uuid);
  //   block_content = get_block_content(block_ptr);

  //   /* Server: send response containing block content */
  //   create_msg_get_block_response(block_ptr->uuid, block_ptr->size, 
  //                                 block_content, &msg, &len);
  //   printf("Server: Sending message (%d bytes):\n", len);
  //   print_msg(msg);
  //   free(block_content);
  //   free(msg);
  // }





  printf("BACS - Server shutting down...\n");

  /* Clean up */
  destroy_meta_t(fs_metadata);

  printf("BACS - exited.\n");
  exit(EXIT_SUCCESS);
}
