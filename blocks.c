/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#include "blocks.h"

/**
 * create_block_t()
 *
 * Allocates and initializes a new block_t structure
 */
block_t *create_block_t()
{
  return NULL;
}



/**
 * destroy_block_t
 *
 * Destroys and frees all memory in the target block_t
 */
void destroy_block_t(block_t *target)
{
  /* Nuke the target */
  free(target);
}
