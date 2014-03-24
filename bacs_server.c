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


#include <stdio.h>
#include <stdlib.h>

#include "file_metadata.h"

int main(int argc, char **argv)
{
  uuid_t *uuids;
  uint64_t num_uuids, i;
  size_t x;

  printf("BACS - Starting file server...\n");

  /* Initialize the file metadata tree */
  fs_metadata = create_meta_t();





  /* THIS IS WHERE WE SHOULD LISTEN FOR MESSAGES */
  printf("BACS - Server ready.\n");


  /* Debugging crap... */
  add_file_meta(fs_metadata, "/awesome/bad/c.txt", 4000, 0, &uuids, &num_uuids);

  printf("Added /a/b/c.txt; UUIDs returned: %" PRIu64 "\n", num_uuids);
  printf("[ ");
  for(i = 0; i < num_uuids; i++) {
    for (x = 0; x < sizeof(uuid_t); x++) {
        if(x == 4 || x == 6 || x == 8 || x == 10)
          printf("-");
        printf("%02x", uuids[i][x]);
    }
    printf(" ");
  }
  printf("]\n");







  printf("BACS - Server shutting down...\n");

  /* Clean up */
  destroy_meta_t(fs_metadata);

  printf("BACS - exited.\n");
  exit(EXIT_SUCCESS);
}
