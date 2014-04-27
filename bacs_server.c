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
#include <unistd.h>

#include "blocks.h"
#include "file_metadata.h"
#include "messages.h"
#include "server_functions.h"




int main(int argc, char **argv)
{
  int opt, port = 0;

  printf("BACS - Starting file server...\n");

  /* Parse command-line arguments for server */
  while((opt = getopt(argc, argv, "h:p:")) != -1) {
    switch(opt) {
      case 'h': 
        /* TODO: Extract IP of other server in cluster */
        break;
      case 'p': 
        /* Extract port to listen on */
        port = atoi(optarg);
        break;
      default: 
        printf("Usage: %s, [-h host_ip] [-p port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  /* Set default port number if necessary */
  if(port == 0) port = SERVER_PORT;

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

  /* TODO: If host IP is provided, retrieve metadata from master */

  /* TODO: Otherwise, we're a master */

  /* Listen for messages */
  printf("BACS - Server ready. Listening on port %d.\n", port);
  start_listening(port);
  printf("BACS - Server shutting down...\n");

  /* Clean up */
  destroy_meta_t(fs_metadata);

  printf("BACS - exited.\n");
  exit(EXIT_SUCCESS);
}
