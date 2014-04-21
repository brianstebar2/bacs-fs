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
#include <string.h>

#include "blocks.h"
#include "common.h"
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
  result->parent = NULL;

  /* Add block to server block list */
  result->next = all_blocks;
  result->prev = NULL;
  if(all_blocks != NULL) all_blocks->prev = result;
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
  char *filename;
  block_t *prev = target->prev;
  block_t *next = target->next;

  /* Ensure that the target block has been marked as deleted */
  if(target->status != DELETED) 
    die_with_error("destroy_block_t - target not marked as deleted");

  /* Delete block file */
  filename = generate_block_filename(target);
  remove(filename);
  free(filename);

  /* Remove block from block list */
  if(prev != NULL) prev->next = next;
  if(next != NULL) next->prev = prev;
  all_blocks_num--;

  /* Nuke the target */
  free(target);
}



/**
 * find_block
 *
 * Returns a pointer to the block_t of the block identified by the specified 
 * UUID if it's present on the local server; Returns NULL otherwise
 */
block_t *find_block(uuid_t uuid)
{
  block_t *current = all_blocks;

  /* Search the block list for a matching UUID */
  while(current != NULL) {
    /* We found a match! */
    if(uuid_compare(uuid, current->uuid) == 0) break;

    /* Keep scanning */
    current = current->next;
  }

  return current;
}



/**
 * generate_block_filename()
 *
 * Generates the filename for a given block
 *
 * NOTE: this method allocates memory for its return value; it is the 
 * responsiblity of the caller to free the allocation
 */
char *generate_block_filename(block_t *target)
{
  char *uuid_string = uuid_str(target->uuid);
  uint16_t filename_length = strlen(uuid_string) + strlen(block_data_path) + 2;
  char *filename = calloc(filename_length, sizeof(char));
  memset(filename, 0, filename_length);
  sprintf(filename, "%s/%s", block_data_path, uuid_string);
  free(uuid_string);

  return filename;
}


/**
 * populate_block()
 *
 * Writes the content to a block and marks it as ready
 */
void populate_block(block_t *target, char *content, int32_t content_size)
{
  char *filename;
  FILE *block_file;

  /* Check if the target block has already been populated or deleted */
  if(target->status != NEW) 
    die_with_error("populate_block - target not a new block\n");

  /* Check that the provided content size matches the block metadata */
  if(target->size != content_size)
    die_with_error("populate_block - content size different than expected\n");

  /* TODO: Check that the content checksum computes properly */


  /* Write the content to disk */
  filename = generate_block_filename(target);
  block_file = fopen(filename, "w");
  fwrite(content, sizeof(char), content_size, block_file);
  fclose(block_file);
  free(filename);

  /* Update status */
  target->status = READY;
}
