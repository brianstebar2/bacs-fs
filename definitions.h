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


 /* ============================ *
  * DEFAULT CONFIGURATION VALUES *
  * ============================ */ 
static uint8_t default_num_replicas = 3;
static uint32_t default_block_size = 1048;

 /* ========= *
  * CONSTANTS *
  * ========= */ 
/* File metadata type constants */
#define FILE 1
#define FOLDER 2

/* File and block metadata status constants */
#define READY 1
#define UPLOADING 2
#define DOWNLOADING 3
#define DELETED 4


/* ======================== *
 * BLOCK METADATA STRUCTURE *
 * ======================== */ 
typedef struct block_s {
  uint32_t size = 0;
  uint8_t status;
  uint32_t checksum = 0; 
  char *data;
} block_t;



 /* ============================== *
  * FILE SYSTEM METADATA STRUCTURE *
  * ============================== */ 
typedef struct meta_s {
  uint8_t type;     /* Can be FILE or FOLDER */
  uint8_t status;   /* Can be any of the status constants */
  char *name;

  // FILE-SPECIFIC METADATA
  uint64_t size = 0;
  uint8_t num_replicas = default_num_replicas;
  uint64_t block_count = 0;
  block_t *blocks;

  // FOLDER-SPECIFIC METADATA
  struct meta_s *files;
  struct meta_s *folders;

  // LINKED LIST POINTER
  struct 
} meta_t;
