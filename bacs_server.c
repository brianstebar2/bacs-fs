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
#include <uuid/uuid.h>
#include "common.h"


#include <stdio.h>
#include <stdlib.h>

#include "file_metadata.h"

int main(int argc, char **argv)
{
  uuid_t *uuids;
  uint64_t num_uuids, i;

  printf("BACS - Starting file server...\n");

  /* Initialize the file metadata tree */
  fs_metadata = create_meta_t();
  fs_metadata->status = READY;





  /* THIS IS WHERE WE SHOULD LISTEN FOR MESSAGES */
  printf("BACS - Server ready.\n");


  /* Debugging crap... */
  add_file_meta(fs_metadata, "/awesome/bad/c.txt", 4000, 0, &uuids, &num_uuids);
  printf("Added /awesome/bad/c.txt; UUIDs returned: %" PRIu64 "\n", num_uuids);
  printf("[ ");
  for(i = 0; i < num_uuids; i++) {
    printf("%s ", uuid_str(uuids[i]));
  }
  printf("]\n");

  printf("\nMETA TREE\n");
  print_meta_tree(fs_metadata, "");
  printf("\n");
  
  add_file_meta(fs_metadata, "/awesome/d.txt", 8000, 0, &uuids, &num_uuids);
  printf("Added /awesome/d.txt; UUIDs returned: %" PRIu64 "\n", num_uuids);
  printf("[ ");
  for(i = 0; i < num_uuids; i++) {
    printf("%s ", uuid_str(uuids[i]));
  }
  printf("]\n");

  printf("\nMETA TREE\n");
  print_meta_tree(fs_metadata, "");
  printf("\n");




  printf("BACS - Server shutting down...\n");

  /* Clean up */
  destroy_meta_t(fs_metadata);

  printf("BACS - exited.\n");
  exit(EXIT_SUCCESS);
}
