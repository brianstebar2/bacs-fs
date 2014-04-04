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
  uint64_t file_size;
  char *msg, *filename;
  uint32_t len;
  meta_t *file_meta;

  printf("BACS - Starting file server...\n");

  /* Initialize the file metadata tree */
  fs_metadata = create_meta_t();
  fs_metadata->status = READY;





  /* THIS IS WHERE WE SHOULD LISTEN FOR MESSAGES */
  printf("BACS - Server ready.\n");



  /* Debugging crap... */
  create_msg_post_file_request("/awesome/bad/c.txt", 4000, &msg, &len);  
  printf("Client: Sending message (%d bytes): \n", len);
  print_msg(msg);
  printf("\n");

  parse_msg_post_file_request(msg, &filename, &file_size);
  free(msg);

  file_meta = add_file_meta(fs_metadata, filename, file_size, 0);
  printf("SERVER META TREE\n");
  print_meta_tree(fs_metadata, "");
  printf("\n");
  free(filename);

  create_msg_post_file_response(file_meta, &msg, &len);
  printf("Server: Sending message (%d bytes): \n", len);
  print_msg(msg);
  free(msg);

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
