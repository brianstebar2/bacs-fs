/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#ifndef CRC32_H
#define CRC32_H

#include <inttypes.h>

uint32_t crc32(uint32_t crc, const void *buf, size_t size);


#endif /* CRC32_H */
