/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#ifndef ERRORS_H
#define ERRORS_H

#include <inttypes.h>


/* =========== *
 * ERROR CODES *
 * =========== */ 
#define NO_ERROR 0

#define ERR_BLOCK_NOT_FOUND 2
#define ERR_MEM_ALLOC 1


/* ========================== *
 * ERROR EXPLANATION FUNCTION *
 * ========================== */ 
const char *error_message(uint8_t err_code);



#endif /* ERRORS_H */
