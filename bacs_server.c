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

int main(int argc, char **argv)
{
  printf("BACS - Starting file server...\n");

  /* Initialize the file metadata tree */
  fs_metadata = create_meta_t();





  /* THIS IS WHERE WE SHOULD LISTEN FOR MESSAGES */
  printf("BACS - Server ready.\n");





  printf("BACS - Server shutting down...\n");

  /* Clean up */
  destroy_meta_t(fs_metadata);

  printf("BACS - exited.\n");
  exit(0);
}
