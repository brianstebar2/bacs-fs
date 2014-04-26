/*
 * Sriram Balasubramaniam
 * Charm Patel
 * Karthik Shankar
 * Brian Stebar II 
 *
 * ECE 6102 - Spring 2014
 * Term Project - Distributed File System
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "blocks.h"
#include "common.h"
#include "errors.h"
#include "file_metadata.h"
#include "messages.h"
#include "server_functions.h"



/**
 * handle_get_block
 * 
 * Returns the content of the specified block
 *
 * NOTE: this method allocates memory for 'response'; it is the responsiblity 
 *       of the caller to free the allocation
 */
void handle_get_block(char *msg, char **response, uint64_t *response_len)
{
  uuid_t uuid;
  block_t *block_ptr;
  char *uuid_string, *block_content;
  uint8_t err_code;

  /* Check message parse for errors */
  err_code = parse_msg_get_block_request(msg, &uuid);
  if(err_code != NO_ERROR) {
    create_msg_error(GET, BACS_BLOCK, err_code, response, response_len);
    goto finish;
  }
    
  /* Look up the block and retrieve its content */
  uuid_string = uuid_str(uuid);
  printf("Retrieving content for block %s \n", uuid_string);
  
  /* Check lookup for errors */
  block_ptr = find_block(uuid);
  if(block_ptr == NULL) {
    create_msg_error(GET, BACS_BLOCK, ERR_BLOCK_NOT_FOUND, 
                     response, response_len);
    goto cleanup_uuid_string;
  }
  block_content = get_block_content(block_ptr);

  /* Create response containing block content */
  err_code = create_msg_get_block_response(block_ptr->uuid, block_ptr->size, 
    block_content, response, response_len);
  
  /* Check message creation for errors */
  if(err_code != NO_ERROR) {
    create_msg_error(GET, BACS_BLOCK, err_code, response, response_len);
    goto cleanup_block_content;
  }

  /* Clean up local data */
  cleanup_block_content: free(block_content);
  cleanup_uuid_string:   free(uuid_string);
  finish: /* do nothing */;
}



/**
 * handle_get_file
 * 
 * Returns the metadata for a specific file
 *
 * NOTE: this method allocates memory for 'response'; it is the responsiblity 
 *       of the caller to free the allocation
 */
void handle_get_file(char *msg, char **response, uint64_t *response_len)
{
  char *filename;
  meta_t *file_meta;
  uint8_t err_code;

  /* Check message parse for errors */
  err_code = parse_msg_get_file_request(msg, &filename);
  if(err_code != NO_ERROR) {
    create_msg_error(GET, BACS_FILE, err_code, response, response_len);
    goto finish;
  }

  /* Lookup file to download */
  printf("Retrieving metadata for file '%s'\n", filename);
  err_code = find_meta(fs_metadata, filename, BACS_FILE, &file_meta);
  
  /* Check lookup for errors */
  if(err_code != NO_ERROR) {
    create_msg_error(GET, BACS_FILE, err_code, response, response_len);
    goto cleanup;
  }
  if(file_meta == NULL) {
    create_msg_error(GET, BACS_FILE, ERR_FILE_NOT_FOUND, response, response_len);
    goto cleanup;
  }

  /* Create response containing UUIDs and IP addresses for file's blocks */
  /* Check message creation for errors */
  err_code = create_msg_get_file_response(file_meta, response, response_len);
  if(err_code != NO_ERROR) {
    create_msg_error(GET, BACS_FILE, err_code, response, response_len);
    goto cleanup;
  }

  /* Clean up local data */
  cleanup: free(filename);
  finish: /* do nothing */;
}



/**
 * handle_get_folder_meta
 * 
 * Returns the metadata of all files and subfolders within the target folder
 *
 * NOTE: this method allocates memory for 'response'; it is the responsiblity 
 *       of the caller to free the allocation
 */
void handle_get_folder_meta(char *msg, char **response, uint64_t *response_len)
{
  char *dirname;
  meta_t *folder_meta;
  uint8_t err_code;

  /* Check message parse for errors */
  err_code = parse_msg_get_folder_meta_request(msg, &dirname);
  if(err_code != NO_ERROR) {
    create_msg_error(GET, BACS_FOLDER, err_code, response, response_len);
    goto finish;
  }

  /* Lookup target folder and check file_meta for error return code */
  printf("Retrieving metadata for folder '%s'\n", dirname);
  err_code = find_meta(fs_metadata, dirname, BACS_FOLDER, &folder_meta);
  if(err_code != NO_ERROR) {
    create_msg_error(GET, BACS_FOLDER, err_code, response, response_len);
    goto cleanup;
  }
  if(folder_meta == NULL) {
    create_msg_error(GET, BACS_FOLDER, ERR_FOLDER_NOT_FOUND, 
      response, response_len);
    goto cleanup;
  }

  /* Create response containing metadata for folder */
  /* Check message creation for errors */
  err_code = create_msg_get_folder_meta_response(folder_meta, response, 
    response_len);
  if(err_code != NO_ERROR) {
    create_msg_error(GET, BACS_FOLDER, err_code, response, response_len);
    goto cleanup;
  }

  /* Clean up local data */
  cleanup: free(dirname);
  finish: /* do nothing */;
}



/**
 * handle_post_block
 * 
 * Populates a block with the content in the message
 *
 * NOTE: this method allocates memory for 'response'; it is the responsiblity 
 *       of the caller to free the allocation
 */
void handle_post_block(char *msg, char **response, uint64_t *response_len)
{
  uint8_t err_code;
  uint32_t block_size;
  uint64_t i;
  char *block_content, *uuid_string;
  uuid_t block_uuid;
  block_t *block_ptr;
  meta_t *file_meta;

  /* Check message parse for errors */
  err_code = parse_msg_post_block_request(msg, &block_uuid, &block_size, 
    &block_content);
  if(err_code != NO_ERROR) {
    create_msg_error(POST, BACS_BLOCK, err_code, response, response_len);
    goto finish;
  }

  /* Lookup block to populate and check lookup for errors */
  uuid_string = uuid_str(block_uuid);
  block_ptr = find_block(block_uuid);
  if(block_ptr == NULL) {
    create_msg_error(POST, BACS_BLOCK, ERR_BLOCK_NOT_FOUND, 
      response, response_len);
    goto cleanup;
  }

  /* Populate block content */
  printf("Populating block %s in file '%s'\n", 
    uuid_string, block_ptr->parent->name);
  err_code = populate_block(block_ptr, block_content, block_size);

  /* Check populate for errors */
  if(err_code != NO_ERROR) {
    create_msg_error(POST, BACS_BLOCK, err_code, response, response_len);
    goto cleanup;
  }
  
  /* Scan through the blocks and see if they're all ready now */
  file_meta = block_ptr->parent;
  for(i=0; i < file_meta->num_blocks; i++)
    if(file_meta->blocks[i]->status != READY) break;

  /* If the scan got through all the blocks, we can mark the file as ready */
  if(i == file_meta->num_blocks) file_meta->status = READY;

  /* Create response indicating success */
  /* Check message creation for errors */
  err_code = create_msg_post_block_response(block_uuid, response, response_len);
  if(err_code != NO_ERROR) {
    create_msg_error(POST, BACS_BLOCK, err_code, response, response_len);
    goto cleanup;
  }


  /* Clean up local data */
  cleanup:
    free(block_content);
    free(uuid_string);
  finish: /*do nothing*/;
}



/**
 * handle_post_file
 * 
 * Adds a file entry to the file system metadata
 *
 * NOTE: this method allocates memory for 'response'; it is the responsiblity 
 *       of the caller to free the allocation
 */
void handle_post_file(char *msg, char **response, uint64_t *response_len)
{
  uint8_t err_code;
  uint64_t file_size;
  char *filename;
  meta_t *file_meta;

  /* Check message parse for errors */
  err_code = parse_msg_post_file_request(msg, &filename, &file_size);
  if(err_code != NO_ERROR) {
    create_msg_error(POST, BACS_FILE, err_code, response, response_len);
    goto finish;
  }

  /* Add metadata for new file and check for error return code */
  printf("Creating metadata for file '%s'\n", filename);
  err_code = add_file_meta(fs_metadata, filename, file_size, 0, &file_meta);
  if(err_code != NO_ERROR) {
    create_msg_error(POST, BACS_FILE, err_code, response, response_len);
    goto cleanup;
  }

  /* Create response containing list of UUIDs for new file's blocks */
  /* Check message creation for errors */
  err_code = create_msg_post_file_response(file_meta, response, response_len);
  if(err_code != NO_ERROR) {
    create_msg_error(POST, BACS_FILE, err_code, response, response_len);
    goto cleanup;
  }

  /* Clean up local data */
  cleanup: free(filename);
  finish: /*do nothing*/;
}



/**
 * handle_post_folder
 * 
 * Adds a folder entry to the file system metadata
 *
 * NOTE: this method allocates memory for 'response'; it is the responsiblity 
 *       of the caller to free the allocation
 */
void handle_post_folder(char *msg, char **response, uint64_t *response_len)
{
  char *dirname;
  meta_t *file_meta;

  /* TODO: Check message parse for errors */
  parse_msg_post_folder_request(msg, &dirname);

  /* Create the new folder */
  /* TODO: Check file_meta for error return code */
  printf("Creating folder '%s'\n", dirname);
  file_meta = add_folder(fs_metadata, dirname);

  /* Create response indicicating success */
  /* TODO: Check message creation for errors */
  create_msg_post_folder_response(response, response_len);

  /* Clean up local data */
  free(dirname);
}



/**
 * start_listening
 * 
 * Listens for and responds to incoming requests
 */
void start_listening()
{
  char *msg, *resp; 
  uint64_t resp_len;




  /* Debugging crap... */
  uuid_t *uuids;
  uint64_t len, num_uuids, num_blocks, i;
  meta_t *file_meta;
  basic_block_t *basic_blocks;
  uuid_t bogus_uuid;

  // /* Client: Send a request to upload a new file */
  // create_msg_post_file_request("/awesome/bad/c.txt", 4096, &msg, &len);  
  // print_msg(msg);
  // handle_post_file(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(msg);

  // printf("SERVER META TREE\n");
  // print_meta_tree(fs_metadata, "");
  // printf("\n");
  
  // /* Client: Take list of UUIDs and send each block */
  // parse_msg_post_file_response(resp, &uuids, &num_uuids);
  // free(resp);
  // for(i=0; i < num_uuids; i++) {
  //   block_t *block_ptr = find_block(uuids[i]);
  //   char block[DEFAULT_BLOCK_SIZE] = {0};
  //   sprintf(block, "Block #%" PRIu64 " content", i);
    
  //   /* Client: send a block */
  //   /* TODO: Add checksum to post block request message */
  //   create_msg_post_block_request(uuids[i], DEFAULT_BLOCK_SIZE, block, &msg, &len);
  //   print_msg(msg);
  //   handle_post_block(msg, &resp, &resp_len);
  //   print_msg(resp);
  //   free(msg);
  //   free(resp);

  //   printf("SERVER FILE META\n");
  //   print_file_meta(block_ptr->parent);
  // }

  // /* Client: create a new folder */
  // create_msg_post_folder_request("/awesome/cool", &msg, &len);
  // print_msg(msg);
  // handle_post_folder(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(msg);
  // free(resp);

  // printf("\nSERVER META TREE\n");
  // print_meta_tree(fs_metadata, "");
  // printf("\n");

  // /* Add another file to the file system */
  // file_meta = add_file_meta(fs_metadata, "/awesome/d.txt", 8000, 0);
  // printf("Added /awesome/d.txt; UUIDs returned: %" PRIu64 "\n", file_meta->num_blocks);

  // printf("\nSERVER META TREE\n");
  // print_meta_tree(fs_metadata, "");
  // printf("\n");

  // /* Have a look at the directory /awesome contents */
  // /* Client: Send a request to the server for folder metadata */
  // create_msg_get_folder_meta_request("/awesome", &msg, &len);
  // print_msg(msg);
  // handle_get_folder_meta(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(msg);
  // free(resp);

  // /* Have a look at the directory /awesome/bad contents */
  // /* Client: Send a request to the server for folder metadata */
  // create_msg_get_folder_meta_request("/awesome/bad", &msg, &len);
  // print_msg(msg);
  // handle_get_folder_meta(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(msg);
  // free(resp);

  // /* Client: Send a message requesting to download a file */
  // create_msg_get_file_request("/awesome/bad/c.txt", &msg, &len);
  // print_msg(msg);
  // handle_get_file(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(msg);

  // /* Client: Take list of UUIDs and request each block */
  // parse_msg_get_file_response(resp, &basic_blocks, &num_blocks);
  // free(resp);
  // for(i=0; i < num_blocks; i++) {
  //   create_msg_get_block_request(basic_blocks[i].uuid, &msg, &len);
  //   print_msg(msg);
  //   handle_get_block(msg, &resp, &resp_len);
  //   print_msg(resp);
  //   free(msg);
  //   free(resp);
  // }

  // /* Now let's test some error messages */
  // /* Try requesting a bogus block */
  // uuid_generate(bogus_uuid);
  // create_msg_get_block_request(bogus_uuid, &msg, &len);
  // print_msg(msg);
  // handle_get_block(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(msg);
  // free(resp);

  // /* Request a bogus file */
  // create_msg_get_file_request("/nonexistent.mp4", &msg, &len);
  // print_msg(msg);
  // handle_get_file(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(resp);
  // free(msg);

  // /* Lookup a bogus directory */
  // create_msg_get_folder_meta_request("/ghost_folder", &msg, &len);
  // print_msg(msg);
  // handle_get_folder_meta(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(msg);
  // free(resp);

  // /* Test POST BLOCK errors */
  // add_file_meta(fs_metadata, "/test.txt", 2000, 0, &file_meta);
  // printf("Added /test.txt; UUIDs returned: %" PRIu64 "\n", file_meta->num_blocks);

  // /* Try posting a bogus block */
  // char block[DEFAULT_BLOCK_SIZE] = {0};
  // sprintf(block, "Bogus content");
  // uuid_generate(bogus_uuid);
  // create_msg_post_block_request(bogus_uuid, DEFAULT_BLOCK_SIZE, block, &msg, &len);
  // print_msg(msg);
  // handle_post_block(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(msg);
  // free(resp);

  // /* Try sending content that's the wrong size */
  // sprintf(block, "Block #0 content");
  // create_msg_post_block_request(file_meta->blocks[0]->uuid, DEFAULT_BLOCK_SIZE/2, block, &msg, &len);
  // print_msg(msg);
  // handle_post_block(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(msg);
  // free(resp);

  // /* Try populating a block twice */
  // create_msg_post_block_request(file_meta->blocks[0]->uuid, DEFAULT_BLOCK_SIZE, block, &msg, &len);
  // print_msg(msg);
  // handle_post_block(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(msg);
  // free(resp);

  // create_msg_post_block_request(file_meta->blocks[0]->uuid, DEFAULT_BLOCK_SIZE, block, &msg, &len);
  // print_msg(msg);
  // handle_post_block(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(msg);
  // free(resp);

  // /* Test POST FILE errors */
  // /* Try sending a relative path */
  // create_msg_post_file_request("file.dat", 4096, &msg, &len);  
  // print_msg(msg);
  // handle_post_file(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(resp);
  // free(msg);

  // /* Try submitting a file twice */
  // create_msg_post_file_request("/file.dat", 4096, &msg, &len);  
  // print_msg(msg);
  // handle_post_file(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(resp);
  // free(msg);

  // create_msg_post_file_request("/file.dat", 4096, &msg, &len);  
  // print_msg(msg);
  // handle_post_file(msg, &resp, &resp_len);
  // print_msg(resp);
  // free(resp);
  // free(msg);







  /* Handle incoming messages */
  /*while(msg_struct = myrecv()) {*/
  // while(1) {
  //   switch(get_header_type(msg)) {
  //     case BACS_REQUEST: 
  //       switch(get_header_resource(msg)) {
          
  //         /* BLOCK requests */
  //         case BACS_BLOCK:  
  //           switch(get_header_action(msg)) {
  //             case GET:   handle_get_block(msg, &resp, &resp_len); break;
  //             case POST:  handle_post_block(msg, &resp, &resp_len); break;
  //             default:    printf("Invalid message action; send error message.\n");
  //           }

  //           break;

  //         /* FILE requests */
  //         case BACS_FILE:  
  //           switch(get_header_action(msg)) {
  //             case GET:   handle_get_file(msg, &resp, &resp_len); break;
  //             case POST:  handle_post_file(msg, &resp, &resp_len); break;
  //             default:    printf("Invalid message action; send error message.\n");
  //           }
  //           break;

  //         /* FOLDER requests */
  //         case BACS_FOLDER:  
  //           switch(get_header_action(msg)) {
  //             case GET:   handle_get_folder_meta(msg, &resp, &resp_len); break;
  //             case POST:  handle_post_folder(msg, &resp, &resp_len); break;
  //             default:    printf("Invalid message action; send error message.\n");
  //           }
  //           break;

  //         default: printf("Unknown message resource; send error message.\n");
  //       }
  //       break;

  //     default: printf("Unknown message type; send error message.\n");
  //   }


  //   /* Send response if necessary */
  //   if(resp_len > 0) {
  //     /* mysend(msg_src_ip, response, response_len); */
  //     print_msg(resp);
  //   }

  //   /* Clean up request and response */    
  //   free(msg);
  //   free(resp);
  //   msg = NULL;
  //   resp = NULL;
  // }
}
