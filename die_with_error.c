/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "die_with_error.h"

void die_with_error(char *error_message)
{
	fprintf(stderr, "%s", error_message);
	if(errno)
		fprintf(stderr, " (error#: %d - %s)\n", errno, strerror(errno));
	fprintf(stderr, "\n");
	
	exit(1);
}
