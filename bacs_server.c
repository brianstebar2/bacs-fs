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
  printf("BACS - Starting file server...\n");

  /* Initialize the file metadata tree */
  create_meta_t(&fs_metadata);
  fs_metadata->status = READY;

  /* Initialize server block storage */
  all_blocks = NULL;
  all_blocks_num = 0;
  block_data_path = DEFAULT_DATA_PATH;

  /* If data folder for blocks doesn't exist, create it */
  mkdir(block_data_path, S_IRWXU | S_IRWXG | S_IRWXO);

  /* TODO: read in blocks already stored */

  /* TODO: retrieve metadata from master */

  /* Listen for messages */
  printf("BACS - Server ready.\n");
  start_listening();
  printf("BACS - Server shutting down...\n");

  /* Clean up */
  destroy_meta_t(fs_metadata);

  printf("BACS - exited.\n");
  exit(EXIT_SUCCESS);
}
