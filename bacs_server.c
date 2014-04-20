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

#include "file_metadata.h"


/* Debugging includdes */
#include <inttypes.h>
#include <uuid/uuid.h>
#include "common.h"
#include "messages.h"


int main(int argc, char **argv)
{
  uint64_t file_size, num_uuids;
  char *msg, *filename;
  uint32_t len, i;
  meta_t *file_meta;
  uuid_t *uuids;

  printf("BACS - Starting file server...\n");

  /* Initialize the file metadata tree */
  fs_metadata = create_meta_t();
  fs_metadata->status = READY;





  /* THIS IS WHERE WE SHOULD LISTEN FOR MESSAGES */
  printf("BACS - Server ready.\n");



  /* Debugging crap... */
  /* Client: Send a request to upload a new file */
  create_msg_post_file_request("/awesome/bad/c.txt", 4000, &msg, &len);  
  printf("Client: Sending message (%d bytes): \n", len);
  print_msg(msg);
  printf("\n");

  /* Server: Add metadata for new file */
  parse_msg_post_file_request(msg, &filename, &file_size);
  free(msg);
  file_meta = add_file_meta(fs_metadata, filename, file_size, 0);
  printf("SERVER META TREE\n");
  print_meta_tree(fs_metadata, "");
  printf("\n");
  free(filename);

  /* Server: send response containing list of UUIDs for new file's blocks */
  create_msg_post_file_response(file_meta, &msg, &len);
  printf("Server: Sending message (%d bytes): \n", len);
  print_msg(msg);
  
  /* Client: Take list of UUIDs and send each block */
  parse_msg_post_file_response(msg, &uuids, &num_uuids);
  free(msg);
  for(i=0; i < num_uuids; i++) {
    uint32_t block_size;
    char *block_content;
    uuid_t block_uuid;
    char block[DEFAULT_BLOCK_SIZE] = {0};
    
    /* Client: send a block */
    sprintf(block, "Block #%d content", i);
    /* TODO: Add checksum to post block request message */
    create_msg_post_block_request(uuids[i], DEFAULT_BLOCK_SIZE, block, &msg, &len);
    printf("Client: Sending message: (%d bytes): \n", len);
    print_msg(msg);

    /* Server: write block and update block metadata */
    parse_msg_post_block_request(msg, &block_uuid, &block_size, &block_content);
    free(msg);
    /* printf("SERVER FILE BLOCK META\n"); */
    /* print_file_block_metas(file_meta); */

    /* Server: send response indicicating success */
    create_msg_post_block_response(uuids[i], &msg, &len);
    printf("Server: Sending message (%d bytes):\n", len);
    print_msg(msg);
  }
  
  /*add_file_meta(fs_metadata, "/awesome/d.txt", 8000, 0);
  printf("Added /awesome/d.txt; UUIDs returned: %" PRIu64 "\n", num_uuids);
  printf("[ ");
  for(i = 0; i < num_uuids; i++) {
    printf("%s ", uuid_str(uuids[i]));
  }
  printf("]\n");

  printf("\nSERVER META TREE\n");
  print_meta_tree(fs_metadata, "");
  printf("\n");*/


  





  printf("BACS - Server shutting down...\n");

  /* Clean up */
  destroy_meta_t(fs_metadata);

  printf("BACS - exited.\n");
  exit(EXIT_SUCCESS);
}
