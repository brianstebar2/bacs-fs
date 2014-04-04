/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#ifndef FILE_METADATA_H
#define FILE_METADATA_H

#include <stdint.h>

#include "definitions.h"

/* Global reference to metadata structure */
meta_t *fs_metadata;



/* ======================== *
 * PUBLIC METHOD PROTOTYPES *
 * ======================== */ 
meta_t *add_file_meta(meta_t *root, char *path, uint64_t size, uint8_t replicas);
void destroy_meta_t(meta_t *target);



/* ========================= *
 * PRIVATE METHOD PROTOTYPES *
 * ========================= */ 
void add_to_meta_tree(meta_t *parent, meta_t *child);
meta_t *create_file(meta_t *parent, const char *name, uint64_t size, 
                    uint8_t replicas);
meta_t *create_meta_t();
meta_t *create_subfolder(meta_t *folder, const char *name);
meta_t *find_child_meta(meta_t *folder, const char *target, uint8_t target_type);
void print_meta_tree(meta_t *root, const char *prefix);



#endif /* FILE_METADATA_H */
