/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdint.h>
#include <uuid/uuid.h>
#include <arpa/inet.h>




/* =================================== *
 * DEFAULT SERVER CONFIGURATION VALUES *
 * =================================== */ 
#define DEFAULT_REPLICAS 3
#define DEFAULT_BLOCK_SIZE 1024 /* bytes */
#define DEFAULT_DATA_PATH "bacs_data"



 /* ========= *
  * CONSTANTS *
  * ========= */ 
/* File metadata type constants */
#define NOTHING 0
#define BACS_FILE_TYPE 1
#define BACS_FOLDER_TYPE 2

/* File and block metadata status constants */
#define NEW 0
#define READY 1
#define UPLOADING 2
#define DOWNLOADING 3
#define DELETED 4

/* Network stack constants */
#define BUFLEN 1500
#define MESSAGE_LEN 1496
#define CLIENT_PORT 9930
#define SERVER_PORT 9931
typedef enum {SUCCESS, FAILURE , RETRY} ErrorCode;



/* ======================== *
 * NETWORK STACK STRUCTURES *
 * ======================== */ 
struct Send_message
{
  struct in_addr hostIP;
  int port_number;
  int seq_number;
  char message[1496];
  int size_of_blocks;
};

struct Node{
	struct Node* previous;
	struct Node* next;
	long IP;
	int PN;
	int size_of_blocks;
	char* message;
	int counter;
}; 

struct Node* head;

/* ========================= *
 * BLOCK METADATA STRUCTURES *
 * ========================= */ 
/* FULL BLOCK STRUCTURE (for use by servers only) */
typedef struct block_s {
  /* BLOCK METADATA */
  uuid_t uuid;         /* Identifer for this block (and its filename) */
  uint32_t size;       /* Bytes of data in this block */
  uint8_t status;      /* Can be any of the status constants */
  /*uint32_t checksum;*/   /* Checksum for verifying block data */

  /* BLOCK HOST METADATA */
  /*uint8_t num_hosts;*/   /* Number of servers hosting this block */
  /*uint32_t *host_ips;*/  /* IP addresses of servers hosting this block */

  /* TREE AND LINKED LIST POINTERS */
  struct meta_s *parent;  /* File to which this block belongs */
  struct block_s *next;   /* Next block in global block list */
  struct block_s *prev;   /* Previos block in global block list */
} block_t;

/* SIMPLIFIED BLOCK STRUCTURE (for use during message passing by both clients 
 * and servers) */
typedef struct basic_block_s {
  uuid_t uuid;         /* Identifer for this block (and its filename) */
  /*uint32_t checksum;*/   /* Checksum for verifying block data */
  uint8_t num_hosts;   /* Number of servers hosting this block */
  /*uint32_t *host_ips;*/  /* IP addresses of servers hosting this block */
} basic_block_t;



 /* =============================== *
  * FILE SYSTEM METADATA STRUCTURES *
  * =============================== */ 
/* FULL METADATA STRUCTURE (for use by servers only) */
typedef struct meta_s {
  /* COMMON METADATA */
  uint8_t type;           /* Can be BACS_FILE_TYPE or BACS_FOLDER_TYPE */
  uint8_t status;         /* Can be any of the status constants */
  char *name;             /* Name of the file or folder */
  uint64_t size;          /* Bytes in file or folder */
  uint32_t version;       /* Number of updates to the entire metadata tree */

  /* FILE-SPECIFIC METADATA */
  uint8_t replicas;       /* Number of times each block should be replicated */
  uint64_t num_blocks;    /* Number of blocks in file (size of *blocks array) */
  block_t **blocks;       /* Array of pointers to block_t's for this file */

  /* FOLDER-SPECIFIC METADATA */
  uint32_t num_files;        /* Number of files in this folder */
  uint32_t num_subfolders;   /* Number of subfolders in this folder */
  struct meta_s *files;      /* Files contained in this folder */
  struct meta_s *subfolders; /* Subfolders of this folder */

  /* TREE AND LINKED LIST POINTERS */
  struct meta_s *parent;  /* Parent folder (NULL if root) */
  struct meta_s *prev;    /* Previous sibling in this folder */
  struct meta_s *next;    /* Next sibling in this folder */
} meta_t;

/* SIMPLIFIED METADATA STRUCTURE (for use during message passing by both clients 
 * and servers) */
typedef struct basic_meta_s {
  uint8_t type;           /* Can be BACS_FILE_TYPE or BACS_FOLDER_TYPE */
  char *name;             /* Name of the file or folder */
  uint64_t size;          /* Bytes in file or folder */
} basic_meta_t;



#endif /* DEFINITIONS_H */
