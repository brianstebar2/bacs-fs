/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#ifndef COMMON_H
#define COMMON_H

#include <inttypes.h>
#include <uuid/uuid.h>


const char *meta_type_string(uint8_t status);
const char *status_string(uint8_t status);
char *strtolower(char *str);
char** str_split(char *a_str, const char a_delim);
char *uuid_str(uuid_t uuid);



#endif /* COMMON_H */
