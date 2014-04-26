/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#include "errors.h"



/**
 * error_message
 *
 * Returns a human-readable string containing an explanation of the specified
 * error code
 */
const char *error_message(uint8_t err_code)
{
  const char *msg;

  switch(err_code) {
    case ERR_BLOCK_NOT_FOUND: msg = "Block not found."; break;
    case ERR_BLOCK_NOT_NEW: msg = "Block has already been populated."; break;
    case ERR_BLOCK_SIZE_MISMATCH:
      msg = "Content size didn't match the block's size."; break;
    case ERR_FILE_EXISTS: msg = "File already exists."; break;
    case ERR_FILE_META_TYPE: 
      msg = "An invalid file metadata type was requested."; break;
    case ERR_FILE_NOT_FOUND: msg = "File not found."; break;
    case ERR_FOLDER_NOT_FOUND: msg = "Folder not found."; break;
    case ERR_HEADER_MISMATCH:
      msg = "A parser received a message with unexpected headers."; break;
    case ERR_INVALID_PATH: 
      msg = "Provided path is not a valid absolute path."; break;
    case ERR_MEM_ALLOC: 
      msg = "Server experienced a memory allocation error."; break;
    case ERR_MSG_ACTION: msg = "Unrecognized action requested."; break;
    case ERR_MSG_RESOURCE: msg = "Unrecognized resource requested."; break;
    case ERR_MSG_TYPE: msg = "Unrecognized message type requested."; break;

    default: msg = "";
  }

  return msg;
}
