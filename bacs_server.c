/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#include <stdlib.h>

#include "file_metadata.h"

int main(int argc, char **argv)
{
  /* Initialize the file metadata tree */
  init_meta_tree(fs_metadata);

  exit(0);
}
