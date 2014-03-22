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

#include "die_with_error.h"
#include "file_metadata.h"



int add_file_meta(meta_t *root, char *name, uint64_t size, uint8_t replicas)
{
  return 0;
}



/**
 * create_meta_t()
 *
 * Allocates and initializes a new meta_t structure
 */
meta_t *create_meta_t()
{
  meta_t *result;

  /* Allocate necessary memory  */
  result = malloc(sizeof(meta_t));
  if(result == NULL) die_with_error("create_meta_t - malloc failed");

  /* Initialize fields */
  result->type = NOTHING;
  result->status = NEW;
  result->name = NULL;
  result->size = 0;
  result->version = 0;

  result->num_replicas = DEFAULT_REPLICAS;
  result->block_count = 0;
  result->blocks = NULL;

  result->num_files = 0;
  result->num_folders = 0;
  result->files = NULL;
  result->folders = NULL;

  result->parent = NULL;
  result->next = NULL;

  /* Return initialized meta_t */
  return result;
}


/**
 * destroy_meta_t
 *
 * Destroys and frees all memory in the target meta_t and recursively destroys
 * and frees its subfolder meta_t's, its file meta_t's, and all its block_t's
 */
void destroy_meta_t(meta_t *target)
{
  /* Destroy and free the blocks for target file */

  /* Destroy and free files within target folder */

  /* Destroy and free subfolders of target folder */

  /* Update target parent's bookkeeping data */

  /* Nuke the target */
  free(target);
}
