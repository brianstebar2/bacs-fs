/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "definitions.h"
#include "die_with_error.h"



/**
 * status_string
 *
 * Translates status codes into string constants
 */
const char *status_string(uint8_t status)
{
  switch(status) {
    case NEW:         return "NEW"; break;
    case READY:       return "READY"; break;
    case UPLOADING:   return "UPLOADING"; break;
    case DOWNLOADING: return "DOWNLOADING"; break;
    case DELETED:     return "DELETED"; break;
    default:          return "UNKNOWN";
  } 
}



/**
 * strtolower
 * 
 * Returns an all-lowercase COPY of str (so free it when you're done with it)
 * NOTE: This assumes str is properly NULL-terminated
 *
 * Borrwed from http://stackoverflow.com/questions/2661766/c-convert-a-mixed-case-string-to-all-lower-case 
 */ 
char *strtolower(char *str)
{
  int i;
  char *new_str = malloc(sizeof(char)*strlen(str));
  if(new_str == NULL) die_with_error("strtolower - malloc failed");

  for(i = 0; str[i]; i++){
    new_str[i] = tolower(str[i]);
  }
  new_str[i] = '\0';

  return new_str;
}



/**
 * str_split
 * 
 * Splits a string on a delimiter (similar to explode() in some scripting 
 * languages); Returns an array of string pointers to the indivdiual tokens, 
 * each of which is a null-terminated string; the returned array of pointers is
 * also null-terminated
 *
 * NOTE: New memory is allocated for each string pointed to in the results
 *       array; each pointed-to string must be individually freed to reclaim
 *       their memory allocation
 *
 * Borrowed from http://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
 */
char** str_split(char* a_str, const char a_delim)
{
  char** result    = 0;
  size_t count     = 0;
  char* tmp        = a_str;
  char* a_str_copy = strdup(a_str);
  /*char* last_delim = 0;*/
  char delim[2];
  delim[0] = a_delim;
  delim[1] = 0;

  /* Count how many elements will be extracted */
  while (*tmp) {
    if (a_delim == *tmp) {
      count++;
      /*last_delim = tmp;*/
    }
    tmp++;
  }

  /* Add space for trailing token (Not needed unless a_str ends with a_delim) */
  /*count += last_delim < (a_str + strlen(a_str) - 1);*/

  /* Add space for terminating null string so caller
     knows where the list of returned strings ends. */
  count++;
  result = malloc(sizeof(char*) * count);
  if(result == NULL) die_with_error("str_split - malloc failed");

  if (result){
    size_t idx  = 0;
    char* token = strtok(a_str_copy, delim);

    while (token) {
      assert(idx < count);
      *(result + idx++) = strdup(token);
      token = strtok(0, delim);
    }
    
    assert(idx == count - 1);
    *(result + idx) = 0;
  }

  free(a_str_copy);
  return result;
}



/**
 * uuid_str
 * 
 * Generates a string representation of the specified uuid_t; Returns a pointer
 * to the new string
 *
 * NOTE: uuid_str allocates memory for the string; the caller is responsible 
 *       for freeing the string 
 */
char *uuid_str(uuid_t uuid)
{
  int i;
  char *tmp;

  /* Allocate the memory for the string and zero it out */
  char *result = malloc(sizeof(char)*37);
  if(result == NULL) die_with_error("uuid_str - malloc failed");
  memset(result, 0, 37);

  /* Build the result string (each byte in the uuid_t is represented as two
     hex characters; hyphens are inserted to improve readability) */
  tmp = result;
  for (i = 0; i < sizeof(uuid_t); i++) {
    if(i == 4 || i == 6 || i == 8 || i == 10) {
      sprintf(tmp, "-");
      tmp = tmp + 1;
    }
    sprintf(tmp, "%02x", uuid[i]);
    tmp = tmp + 2;
  }

  return result;
}