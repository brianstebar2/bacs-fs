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
#include <uuid/uuid.h>


 /* ============================ *
  * DEFAULT CONFIGURATION VALUES *
  * ============================ */ 
static uint8_t default_num_replicas = 3;
static uint32_t default_block_size = 1048;
static char default_data_folder[] = "~/bacs_data";



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
  uuid_t uuid;        /* Identifer for this block (and its filename) */
  uint32_t size;      /* Bytes of data in this block */
  uint8_t status;     /* Can be any of the status constants */
  uint32_t checksum;  /* Checksum for verifying block data */
} block_t;



 /* ============================== *
  * FILE SYSTEM METADATA STRUCTURE *
  * ============================== */ 
typedef struct meta_s {
  /* COMMON METADATA */
  uint8_t type;           /* Can be FILE or FOLDER */
  uint8_t status;         /* Can be any of the status constants */
  char *name;             /* Name of the file or folder */
  uint64_t size;          /* Bytes in file or folder */
  uint32_t version;       /* Number of updates to the entire metadata tree */

  /* FILE-SPECIFIC METADATA */
  uint8_t num_replicas;   /* Number of times each block should be replicated */
  uint64_t block_count;   /* Number of blocks in file (size of *blocks array) */
  block_t *blocks;        /* block_t's for this file */

  /* FOLDER-SPECIFIC METADATA */
  struct meta_s *files;   /* Files contained in this folder */
  struct meta_s *folders; /* Subfolders of this folder */

  /* LINKED LIST POINTERS */
  struct meta_s *next;    /* Point to next meta_t in this folder */
} meta_t;
