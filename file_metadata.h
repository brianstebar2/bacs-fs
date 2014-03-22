/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#include <stdint.h>

#include "definitions.h"

/* Global reference to metadata structure */
meta_t *fs_metadata;


int add_file_meta(meta_t *root, char *name, uint64_t size, uint8_t replicas);
meta_t *create_meta_t();
void destroy_meta_t(meta_t *target);

