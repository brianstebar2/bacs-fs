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
#include <inttypes.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "blocks.h"
#include "common.h"
#include "die_with_error.h"
#include "errors.h"
#include "file_metadata.h"



/**
 * add_file_meta
 *
 * Adds the specified file to the specified file metadata tree; creates the
 * folders in the file's path if nessary; creates empty blocks for the new file;
 * returns a pointer to the new file's meta_t structure; returns NO_ERROR or an
 * error code
 *
 * root      - the root of the metadata tree that will contain the new metadata
 * path      - the target fully-qualified path (including the file's name)
 * size      - number of bytes in the new file
 * replicas  - minimum number of replicas this file should have; if 0, then
 *             the default number of replicas will be used (see definitions.h)
 * 
 * NOTE: If the file in 'path' already exists, an exception is thrown
 */
uint8_t add_file_meta(meta_t *root, char *path, uint64_t size, uint8_t replicas, 
                      meta_t **return_meta)
{
  char *lower_path, **path_parts;
  meta_t *current_folder = root, *new_file, *old_file;
  uint64_t i = 0;
  uint8_t err_code = NO_ERROR;

  /* Make sure path starts with a '/' */
  if(strncmp(path, "/", 1) != 0) return ERR_INVALID_PATH;

  /* Make sure we the file doesn't already exist (check for errors) */
  err_code = find_meta(current_folder, path, BACS_FILE_TYPE, &old_file);
  if(err_code != NO_ERROR) return err_code;
  if(old_file != NULL) return ERR_FILE_EXISTS;

  /* Parse full path contained in name */
  lower_path = strtolower(path);
  path_parts = str_split(lower_path, '/');

  /* If necessary, navigate to correct folder within metadata; to tell if
     path_parts[i] is a file or folder name, check the next index of path_parts
     for a null terminator */
  while(path_parts[i+1]) {
    meta_t *subfolder;
    /* Check find_child_meta for error codes */
    err_code = find_child_meta(current_folder, path_parts[i], BACS_FOLDER_TYPE, 
      &subfolder);
    if(err_code != NO_ERROR) goto failure_exit;

    /* If the target folder doesn't exist, create it */
    if(subfolder == NULL) {
      /* Check create_subfolder for error codes */
      err_code = create_subfolder(current_folder, path_parts[i], &subfolder);
      if(err_code != NO_ERROR) goto failure_exit;
    }
    
    /* Switch into target folder */
    current_folder = subfolder;

    /* Clean up after str_split */
    free(path_parts[i]);
    i++;

    /* When this loop exits, path_parts[i] will point at the filename token,
       and current_folder will point to the destination folder */
  }

  /* Create metadata for this file and check for error codes */
  err_code = create_file(current_folder, path_parts[i], size, replicas, 
    &new_file);
  if(err_code != NO_ERROR) goto failure_exit;

  /* Clean up and return */
  free(lower_path);
  *return_meta = new_file;
  return NO_ERROR;

  /* We only get here if an error code was detected above */
  failure_exit:
    free(lower_path);
    while(path_parts[i]) {
      free(path_parts[i]);
      i++;
    }
    return err_code;
}



/**
 * add_folder
 *
 * Adds the specified folder to the specified file metadata tree; creates all
 * folders in the path as nessary; Assigns a pointer to the new folder's meta_t 
 * structure to 'return_meta'; Returns NO_ERROR or an error code
 *
 * root      - the root of the metadata tree that will contain the new metadata
 * path      - the target fully-qualified path (including the file's name)
 * return_meta - (return value) pointer to the new folder's meta_t
 */
uint8_t add_folder(meta_t *root, char *path, meta_t **return_meta)
{
  char *lower_path, **path_parts;
  meta_t *current_folder = root, *subfolder;
  uint64_t i = 0;
  uint8_t err_code;

  /* Make sure path starts with a '/' */
  if(strncmp(path, "/", 1) != 0) return ERR_INVALID_PATH;

  /* If the folder already exists, we have nothing to do */
  /* Check find_meta for error */
  err_code = find_meta(current_folder, path, BACS_FOLDER_TYPE, &subfolder);
  if(err_code != NO_ERROR) return err_code;
  if(subfolder != NULL) {
    *return_meta = subfolder;
    return NO_ERROR;
  }

  /* Parse full path contained in name */
  lower_path = strtolower(path);
  path_parts = str_split(lower_path, '/');

  /* If necessary, navigate to correct folder within metadata */
  while(path_parts[i]) {
    /* Check for error codes */
    err_code = find_child_meta(current_folder, path_parts[i], BACS_FOLDER_TYPE, 
      &subfolder);
    if(err_code != NO_ERROR) goto failure_exit;

    /* If the target folder doesn't exist, create it */
    if(subfolder == NULL) {
      /* Check for error codes */
      err_code = create_subfolder(current_folder, path_parts[i], &subfolder);
      if(err_code != NO_ERROR) goto failure_exit;
    }
    
    /* Switch into target folder */
    current_folder = subfolder;

    /* Clean up after str_split */
    free(path_parts[i]);
    i++;
  }

  *return_meta = subfolder;
  return NO_ERROR;

  /* We only get here if an error code was detected above */
  failure_exit:
    free(lower_path);
    while(path_parts[i]) {
      free(path_parts[i]);
      i++;
    }
    return err_code;
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
  
  if(child->type == BACS_FOLDER_TYPE) {
    child->next = parent->subfolders;
    if(parent->subfolders != NULL) parent->subfolders->prev = child;
    parent->subfolders = child; 
  } else {
    child->next = parent->files;
    if(parent->files != NULL) parent->files->prev = child;
    parent->files = child;
  }

  /* Update bookeeping data of all parents in the tree */
  if(child->type == BACS_FOLDER_TYPE) { 
    parent->num_subfolders++; 
  }
  else { 
    parent->num_files++;
    parent->size = parent->size + child->size;
  }

  tmp = parent;
  while(tmp) {
    tmp->version = version;
    tmp = tmp->parent;
  }
}



/**
 * create_file
 *
 * Creates a new file in the 'parent' folder and generates corresponding
 * empty blocks for the file; Assigns 'return_meta' to point to the new file;
 * Returns NO_ERROR or an error code
 *
 * root        - the root of metadata tree that will contain the new metadata
 * path        - the target fully-qualified path (including the file's name)
 * size        - number of bytes in the new file
 * replicas    - minimum number of replicas this file should have; if 0, then
 *               the default number of replicas will be used (see definitions.h)
 * return_meta - (return value) pointer to the new file meta_t
 *
 * NOTE: create_file copies 'name's value into a new string, so freeing the 
 *       string that 'name' points to after creating the folder is safe.
 * NOTE: NOT THREAD SAFE
 */
uint8_t create_file(meta_t *parent, const char *name, uint64_t size, 
                    uint8_t replicas, meta_t **return_meta)
{
  meta_t *new_file;
  uint64_t i, bytes_left, x;
  uint8_t err_code;

  /* Create new meta_t and check for error codes */
  err_code = create_meta_t(&new_file);
  if(err_code != NO_ERROR) return err_code;

  /* Populate the fields in the new file's metadata */
  new_file->type = BACS_FILE_TYPE;
  new_file->status = UPLOADING;
  new_file->name = strdup(name);
  new_file->size = size;
  new_file->replicas = replicas ? replicas : DEFAULT_REPLICAS;

  /* Calculate the number of blocks for this file and allocate its array of
     block pointers */
  new_file->num_blocks = 
    (size / DEFAULT_BLOCK_SIZE) + ((size % DEFAULT_BLOCK_SIZE) > 0);
  new_file->blocks = malloc(sizeof(block_t *) * new_file->num_blocks);
  if(new_file->blocks == NULL) {
    err_code = ERR_MEM_ALLOC;
    goto failure_cleanup1;
  }

  /* Generate blocks for this file */
  bytes_left = size;
  for(i = 0; i < new_file->num_blocks; i++) {
    err_code = create_block_t(&new_file->blocks[i]);
    if(err_code != NO_ERROR) goto failure_cleanup2;

    /* Populate block fields */
    new_file->blocks[i]->parent = new_file;
    new_file->blocks[i]->size = 
      (bytes_left > DEFAULT_BLOCK_SIZE) ? DEFAULT_BLOCK_SIZE : bytes_left;
    bytes_left = bytes_left - new_file->blocks[i]->size;
  }

  /* Add file to parent's file list */
  add_to_meta_tree(parent, new_file);

  *return_meta = new_file;
  return NO_ERROR;

  /* These steps will only be reached when error codes are detected */
  failure_cleanup2:
    for(x = i; x < i; x++) destroy_block_t(new_file->blocks[x]);
  failure_cleanup1:
    free(new_file->name);
    free(new_file);

    return err_code;
}



/**
 * create_meta_t()
 *
 * Allocates and initializes a new meta_t structure at 'return_meta'; Returns
 * NO_ERROR or an error code
 */
uint8_t create_meta_t(meta_t **return_meta)
{
  meta_t *result;

  /* Allocate necessary memory  */
  result = malloc(sizeof(meta_t));
  if(result == NULL) return ERR_MEM_ALLOC;

  /* Initialize fields */
  result->type = NOTHING;
  result->status = NEW;
  result->name = NULL;
  result->size = 0;
  result->version = 0;

  result->replicas = 0;
  result->num_blocks = 0;
  result->blocks = NULL;

  result->num_files = 0;
  result->num_subfolders = 0;
  result->files = NULL;
  result->subfolders = NULL;

  result->parent = NULL;
  result->prev = NULL;
  result->next = NULL;

  /* Return initialized meta_t */
  *return_meta = result;
  return NO_ERROR;
}



/**
 * create_subfolder
 *
 * Creates a new subfolder with the specified name and adds it to 'folder's
 * list of subfolders; Sets 'return_meta' to point to new folder's meta_t;
 * Returns NO_ERROR or an error code
 *
 * NOTE: create_subfolder copies 'name's value into a new string, so freeing
 *       the string that 'name' points to after creating the folder is safe.
 * NOTE: NOT THREAD SAFE
 */
uint8_t create_subfolder(meta_t *parent, const char *name, meta_t **return_meta)
{
  uint8_t err_code;
  meta_t *new_folder;

  /* Check for error codes */
  err_code = create_meta_t(&new_folder);
  if(err_code != NO_ERROR) return err_code;

  /* Populate the fields of the new folder's meta_t */
  new_folder->type = BACS_FOLDER_TYPE;
  new_folder->status = READY;
  new_folder->name = strdup(name);

  /* Add subfolder to the parent file tree */
  add_to_meta_tree(parent, new_folder);

  *return_meta = new_folder;
  return NO_ERROR;
}



/**
 * destroy_meta_t
 *
 * Destroys and frees all memory in the target meta_t and recursively destroys
 * and frees its subfolder meta_t's, its file meta_t's, all its block_t's, and 
 * its strings; Returns NO_ERROR or an error code
 */
uint8_t destroy_meta_t(meta_t *target)
{
  uint8_t err_code;
  uint64_t i;
  meta_t *current;

  /* Make sure that neither this file nor its children are being downloaded */
  /* NOTE: We want to make sure we detect errors before we actually delete
   *       anything; half-way deleting a folder and then finding an error would 
   *       be a Very Bad Thing (TM). */
  if(find_busy_child(target) != NULL) return ERR_FILE_BUSY;

  /* Destroy and free files and subfolders within target folder */
  /* NOTE: Again, check for error codes before we make any permanent changes */
  if(target->type == BACS_FOLDER_TYPE) {
    /* Recursively destroy all subfolders within this folder */
    current = target->subfolders;
    while(current != NULL) {
      err_code = destroy_meta_t(current);
      if(err_code != NO_ERROR) return err_code;
      current = current->next;
    }

    /* Recursively destroy all files in this folder */
    current = target->files;
    while(current != NULL) {
      err_code = destroy_meta_t(current);
      if(err_code != NO_ERROR) return err_code;
      current = current->next;
    }
  }

  /* Now we can start making changes; Mark file as and its blocks as deleted (so 
   * nobody starts downloading it) */
  target->status = DELETED;
  for(i=0; i < target->num_blocks; i++) target->blocks[i]->status = DELETED;

  /* Destroy and free the blocks for target file */
  /* TODO: The rest of this process could be done asynchronously to make 
   * destroy_meta_t faster */
  for(i=0; i < target->num_blocks; i++) {
    /* Check for error codes */
    err_code = destroy_block_t(target->blocks[i]);
    if(err_code != NO_ERROR) return err_code;
    target->blocks[i] = NULL;
  }

  /* Remove the file from the file tree */
  remove_from_meta_tree(target);

  /* Nuke the target */
  free(target);

  return NO_ERROR;
}



/**
 * find_child_meta
 *
 * Returns the first meta_t in the tree with a status of DOWNLOADING
 */
meta_t *find_busy_child(meta_t *parent) 
{
  meta_t *result, *current;

  /* Scan the files of this meta_t */
  current = parent->files;
  while(current != NULL) {
    if(current->status == DOWNLOADING) return current;
    current = current->next;
  }

  /* Scan the subfolders of this meta_t */
  current = parent->subfolders;
  while(current != NULL) {
    result = find_busy_child(current);
    if(result != NULL) return result;
    current = current->next;
  }

  /* If we made it this far, we didn't find any busy meta_t's */
  return NULL;
}



/**
 * find_child_meta
 *
 * Locates the target subfolder/file within the 'folder' if it exists; If target
 * is found, a pointer is saved in 'return_meta'; Otherwise, 'return_meta' is
 * set to NULL; Returns NO_ERROR or an error code
 */
uint8_t find_child_meta(meta_t *folder, const char *target, uint8_t target_type,
                        meta_t **return_meta)
{
  meta_t *result;

  if(target_type == BACS_FOLDER_TYPE)
    result = folder->subfolders;
  else if(target_type == BACS_FILE_TYPE)
    result = folder->files;
  else return ERR_FILE_META_TYPE;

  /* Iterate through child list until either a match is found or we reach
   * the end of the list */
  while(result) {
    if(strcmp(target, result->name) == 0) break;
    result = result->next;
  }

  *return_meta = result;
  return NO_ERROR;
}



/**
 * find_meta
 *
 * Locates the meta_t of the target subfolder/file within the 'folder' or any of 
 * its children if it exists; if the target is found, 'return_meta' is assigned
 * its pointer; Otherwise, 'return_meta' is set to NULL; Returns NO_ERROR or an
 * error code
 */
uint8_t find_meta(meta_t *folder, char *path, uint8_t target_type, 
                  meta_t **return_meta)
{
  uint8_t err_code;
  int i = 0;
  meta_t *current_meta = folder;
  char *lower_path, **path_parts;

  /* Make sure path starts with a '/' */
  if(strncmp(path, "/", 1) != 0) return ERR_INVALID_PATH;

  /* Parse full path contained in name */
  lower_path = strtolower(path);
  path_parts = str_split(lower_path, '/');

  /* If necessary, navigate to correct folder within metadata; to tell if
     path_parts[i] is a file or folder name, check the next index of path_parts
     for a null terminator */
  while(path_parts[i] && path_parts[i+1]) {
    if(current_meta != NULL) {
      /* TODO: Check find_child_meta for error codes */
      err_code = find_child_meta(current_meta, path_parts[i], BACS_FOLDER_TYPE, 
        &current_meta);
      if(err_code != NO_ERROR) goto failure_exit;
    }

    /* Clean up after str_split */
    free(path_parts[i]);
    i++;

    /* When this loop exits, path_parts[i] will point at the last token,
       and current_meta will point to the folder containing the last token */
    fprintf(stderr, "done\n");
  }

  /* Check for the last token based on the target type */
  if(current_meta != NULL && path_parts[i] != NULL) {
    /* Check find_child_meta for error codes */
    err_code = find_child_meta(current_meta, path_parts[i], target_type, 
      &current_meta);
    if(err_code != NO_ERROR) goto failure_exit;
  }

  free(path_parts[i]);
  *return_meta = current_meta;
  return NO_ERROR;

  /* We only get here if an error code was detected above */
  failure_exit:
    free(lower_path);
    while(path_parts[i]) {
      free(path_parts[i]);
      i++;
    }
    return err_code;
}



/**
 * print_file_meta
 *
 * DEBUGGING; Prints out a representation of the specified file tree
 */
void print_file_meta(meta_t *file_meta)
{
  uint64_t i;

  /* Print the file's metadata */
  printf("v%d:%s - %s, %" PRIu64 " bytes, %" PRIu64 " blocks, %d replicas\n",
    file_meta->version, file_meta->name, status_string(file_meta->status), 
    file_meta->size, file_meta->num_blocks, file_meta->replicas);

  /* Print status of each block in the file */
  for(i=0; i < file_meta->num_blocks; i++) {
    block_t *block = file_meta->blocks[i];
    if(block != NULL) {
      char *uuid_string = uuid_str(block->uuid);
      printf(" - %s: %s, %d bytes\n", uuid_string, status_string(block->status),
        block->size);
      free(uuid_string);
    }
    else printf(" - (BLOCK NOT FOUND)\n");
  }

  printf("\n");
}



/**
 * print_meta_tree
 *
 * DEBUGGING; Prints out a representation of the specified file tree
 */
void print_meta_tree(meta_t *folder, const char *prefix)
{
  meta_t *ptr = NULL;
  char *new_prefix;

  /* Print out this folder's name */
  printf("%sv%d:FOLDER:%s - %s, %" PRIu64 " bytes\n", prefix, folder->version, 
    folder->name ? folder->name : "<root>", status_string(folder->status),
    folder->size);

  /* Generate new prefix for recursive calls */
  new_prefix = calloc(strlen(prefix) + 3, sizeof(char));
  if(strlen(prefix) == 0)   
    sprintf(new_prefix, "|--");
  else
    sprintf(new_prefix, "|  %s", prefix);
  
  /* Print out this folder's subfolders */
  ptr = folder->subfolders;
  while(ptr != NULL) {
    print_meta_tree(ptr, new_prefix);
    ptr = ptr->next;
  }

  /* Print out this folder's files */
  ptr = folder->files;
  while(ptr != NULL) {
    printf("%sv%d:FILE:%s - %s, %" PRIu64 " bytes, %" PRIu64 " blocks, %d replicas\n",
      new_prefix, ptr->version, ptr->name, status_string(ptr->status), 
      ptr->size, ptr->num_blocks, ptr->replicas);
    ptr = ptr->next;
  }

  free(new_prefix);
}



/**
 * remove_from_meta_tree
 *
 * Removes 'target' from its file tree; updates all the bookeeping variables 
 * of all the parents
 *
 * NOTE: NOT THREAD SAFE
 */
void remove_from_meta_tree(meta_t *target)
{
  meta_t *tmp = target->parent;
  uint32_t version;

  /* Find the file tree's root version number */
  while(tmp) {
    version = tmp->version;
    tmp = tmp->parent;
  }

  /* Remove the target from its parent's children list */
  if(target->prev != NULL) target->prev->next = target->next;
  if(target->next != NULL) target->next->prev = target->prev;
  if(target->parent != NULL) { /* Root node's parent will be NULL */
    if(target->parent->files == target) 
      target->parent->files = target->next;
    if(target->parent->subfolders == target) 
      target->parent->subfolders = target->next;
  }

  /* Update bookeeping data of all parents in the tree */
  if(target->parent != NULL) {
    if(target->type == BACS_FOLDER_TYPE) { 
      target->parent->num_subfolders--; 
    }
    else { 
      target->parent->num_files--;
      target->parent->size = target->parent->size - target->size;
    }
  }

  /* Increment the version throughout the tree */
  version++;
  tmp = target->parent;
  while(tmp) {
    tmp->version = version;
    tmp = tmp->parent;
  }
}
