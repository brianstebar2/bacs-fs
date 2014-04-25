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
    case ERR_MEM_ALLOC: 
      msg = "Server experienced a memory allocation error."; break;

    default: msg = "";
  }

  return msg;
}
