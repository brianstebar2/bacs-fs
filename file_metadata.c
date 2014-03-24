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
#include "file_metadata.h"



/**
 * add_file_meta
 *
 * Adds the specified file to the specified file metadata tree; creates the
 * folders in the file's path if nessary; creates empty blocks for the new file;
 * returns a pointer to an array of block UUIDs and its size
 *
 * root      - the root of the metadata tree that will contain the new metadata
 * path      - the target fully-qualified path (including the file's name)
 * size      - number of bytes in the new file
 * replicas  - minimum number of replicas this file should have; if 0, then
 *             the default number of replicas will be used (see definitions.h)
 * uuids     - (return val) pointer to where the list of block UUIDs is stored
 * num_uuids - (return val) size of the UUIDs array; If 0 is returned, an error
 *             occurred
 */
void add_file_meta(meta_t *root, char *path, uint64_t size, uint8_t replicas,
                   uuid_t **uuids, uint64_t *num_uuids)
{
  char *lower_path, **path_parts;
  meta_t *current_folder = root;
  int i = 0;

  /* Make sure path starts with a '/' */
  if(strncmp(path, "/", 1) != 0) 
    die_with_error("add_file_meta - invalid path specified");

  /* Set num_uuids to 0 for now; if nothing blows up, we'll set it right later */
  *num_uuids = 0;

  /* Parse full path contained in name */
  lower_path = strtolower(path);
  path_parts = str_split(lower_path, '/');

  /* If necessary, navigate to correct folder within metadata; to tell if
     path_parts[i] is a file or folder name, check the next index of path_parts
     for a null terminator */
  while(path_parts[i+1]) {
    meta_t *subfolder;
    fprintf(stderr, " - folder name %d: '%s'\n", i, path_parts[i]);
    subfolder = find_child_meta(current_folder, path_parts[i], FOLDER);

    /* If the target folder doesn't exist, create it */
    if(subfolder == NULL) {
      fprintf(stderr, " - subfolder not found; creating one\n");
      subfolder = create_subfolder(current_folder, path_parts[i]);
    } else {
      fprintf(stderr, " - found subfolder '%s'\n", subfolder->name);
    }
    
    /* Switch into target folder */
    current_folder = subfolder;

    /* Clean up after str_split */
    free(path_parts[i]);
    i++;

    /* When this loop exits, path_parts[i] will point at the filename token,
       and current_folder will point to the destination folder */
  }
  fprintf(stderr, " - file name: '%s'\n", path_parts[i]);

  /* Create metadata for this file */


  /* Pass back the list of block UUIDs for this file */

  /* Clean up */
  free(lower_path);
}



/**
 * add_to_meta_tree
 *
 * Adds 'child' as a child to 'parent' in the file tree; updates all the 
 * bookeeping variables of all the parents
 *
 * NOTE: NOT THREAD SAFE
 */
void add_to_meta_tree(meta_t *parent, meta_t *child)
{
  meta_t *tmp = parent;
  uint32_t version;

  /* Find the file tree's root version number */
  while(tmp) {
    version = tmp->version;
    tmp = tmp->parent;
  }

  /* Version the child and add it to the tree */
  version++;
  child->version = version;
  child->parent = parent;
  
  if(child->type == FOLDER) {
    child->next = parent->subfolders; 
    parent->subfolders = child; 
  } else {
    child->next = parent->files;
    parent->files = child;
  }

  /* Update bookeeping data of all parents in the tree */
  tmp = parent;
  while(tmp) {
    tmp->version = version;
    
    if(child->type == FOLDER) { 
      tmp->num_folders++; 
    }
    else { 
      tmp->num_files++;
      tmp->size = tmp->size + child->size;
    }

    tmp = tmp->parent;
  }
}



/**
 * create_file
 *
 * Creates a new file in the 'parent' folder and generates corresponding
 * empty blocks for the file; Returns a pointer to the new file
 *
 * root      - the root of the metadata tree that will contain the new metadata
 * path      - the target fully-qualified path (including the file's name)
 * size      - number of bytes in the new file
 * replicas  - minimum number of replicas this file should have; if 0, then
 *             the default number of replicas will be used (see definitions.h)
 *
 * NOTE: create_file copies 'name's value into a new string, so freeing the 
 *       string that 'name' points to after creating the folder is safe.
 * NOTE: NOT THREAD SAFE
 */
meta_t *create_file(meta_t *parent, const char *name, uint64_t size, 
                    uint8_t replicas)
{
  meta_t *new_file = create_meta_t();

  /* Populate the fields in the new file's metadata */
  new_file->type = FILE;
  new_file->status = UPLOADING;
  new_file->name = strdup(name);
  new_file->size = size;
  new_file->replicas = replicas;

  /* Generate blocks for this file */
  /*fprintf(stderr, "create_file: ");*/

  /* Add file to parent's file list */
  add_to_meta_tree(parent, new_file);

  return new_file;
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

  result->replicas = DEFAULT_REPLICAS;
  result->block_count = 0;
  result->blocks = NULL;

  result->num_files = 0;
  result->num_folders = 0;
  result->files = NULL;
  result->subfolders = NULL;

  result->parent = NULL;
  result->next = NULL;

  /* Return initialized meta_t */
  return result;
}



/**
 * create_subfolder
 *
 * Creates a new subfolder with the specified name and adds it to 'folder's
 * list of subfolders; Returns a pointer to the new subfolder
 *
 * NOTE: create_subfolder copies 'name's value into a new string, so freeing
 *       the string that 'name' points to after creating the folder is safe.
 * NOTE: NOT THREAD SAFE
 */
meta_t *create_subfolder(meta_t *parent, const char *name)
{
  meta_t *new_folder = create_meta_t();

  /* Populate the fields of the new folder's meta_t */
  new_folder->type = FOLDER;
  new_folder->status = READY;
  new_folder->name = strdup(name);

  /* Add subfolder to the parent file tree */
  add_to_meta_tree(parent, new_folder);

  return new_folder;
}


/**
 * destroy_meta_t
 *
 * Destroys and frees all memory in the target meta_t and recursively destroys
 * and frees its subfolder meta_t's, its file meta_t's, all its block_t's, and 
 * its strings
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



/**
 * find_child_meta
 *
 * Returns a pointer to the meta_t of the target subfolder/file within the
 * 'folder' if it exists; Returns NULL otherwise
 */
meta_t *find_child_meta(meta_t *folder, const char *target, uint8_t target_type)
{
  meta_t *result = folder->subfolders;

  /* Iterate through child list until either a match is found or we reach
   * the end of the list */
  fprintf(stderr, "find_child_meta('%s', '%s', %d)\n", folder->name, target, target_type);
  while(result) {
    fprintf(stderr, " - checking '%s'\n", result->name);
    if(strcmp(target, result->name) == 0) break;
    result = result->next;
  }

  return result;
}
