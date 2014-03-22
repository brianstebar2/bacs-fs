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
#define DEFAULT_REPLICAS 3
#define DEFAULT_BLOCK_SIZE 1048 /* bytes */
#define DEFAULT_DATA_PATH "~/bacs_data"



 /* ========= *
  * CONSTANTS *
  * ========= */ 
/* File metadata type constants */
#define NOTHING 0
#define FILE 1
#define FOLDER 2

/* File and block metadata status constants */
#define NEW 0
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
  uint32_t num_files;     /* Number of files in this folder */
  uint32_t num_folders;   /* Number of subfolders in this folder */
  struct meta_s *files;   /* Files contained in this folder */
  struct meta_s *folders; /* Subfolders of this folder */

  /* TREE AND LINKED LIST POINTERS */
  struct meta_s *parent;  /* Parent folder (NULL if root) */
  struct meta_s *next;    /* Next sibling in this folder */
} meta_t;
