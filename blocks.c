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

#include "blocks.h"
#include "die_with_error.h"

/**
 * create_block_t()
 *
 * Allocates and initializes a new block_t structure
 */
block_t *create_block_t()
{
  block_t *result;

  /* Allocate necessary memory */
  result = malloc(sizeof(block_t));
  if(result == NULL) die_with_error("create_block_t - malloc failed");

  /* Initialize fields */
  uuid_generate(result->uuid);
  result->size = 0;
  result->status = NEW;
  result->checksum = 0;

  /* Add block to server block list */
  result->next = all_blocks;
  result->prev = NULL;
  all_blocks->prev = result;
  all_blocks = result;
  all_blocks_num++;

  return result;
}



/**
 * destroy_block_t
 *
 * Destroys and frees all memory in the target block_t; Deletes the block's 
 * backing file
 */
void destroy_block_t(block_t *target)
{
  block_t *prev = target->prev;
  block_t *next = target->next;

  /* Delete block file */

  /* Remove block from block list */
  if(prev != NULL) prev->next = next;
  if(next != NULL) next->prev = prev;

  /* Nuke the target */
  free(target);
}



/**
 * populate_block()
 *
 * Writes the content to a block and marks it as ready
 */
void populate_block(block_t *target, char *content)
{

}
