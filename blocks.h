/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#ifndef BLOCKS_H
#define BLOCKS_H



#include "definitions.h"

/* Global reference to server's block list */
uint64_t all_blocks_num;
block_t *all_blocks;

/* Block file path information */
const char *block_data_path;



block_t *create_block_t();
void destroy_block_t(block_t *target);
block_t *find_block(uuid_t uuid);
char *generate_block_filename(block_t *target);
char *get_block_content(block_t *target);
void populate_block(block_t *target, char *content, int32_t content_size);



#endif /* BLOCKS_H */
