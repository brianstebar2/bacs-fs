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
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include "UDPserver.h"
#include <sys/stat.h>
#include "messages.h"
#include "send_file.h"
#include "definitions.h"
#include "die_with_error.h"

bool change_dir(char* dir, char* path, bool l)
{
	if(l==0)
	{
  		char dirpath[1024];
  		int result;

		strcpy(dirpath,path);
  		strcat(dirpath,"/");
  		strcat(dirpath,dir);
  		/*printf("dirpath:%s\n",dirpath);*/
  		result = chdir(dirpath);
  		if(result==0)
  		{
    			strcat(path,"/");
    			strcat(path,dir);
    			printf("Changed to directory '%s'\n", path);
  		}
  		else 
  		{
			perror("cd() error");
			return 1;
  		}
	}
	else
	{
		char dirpath[1024];
		char* msg = 0;
		uint64_t msg_len;
		uint32_t num_metas;
		int i;
		basic_meta_t *basic_metas = 0;
  		strcpy(dirpath,path);
  		strcat(dirpath,"/");
  		strcat(dirpath,dir);
  		/*printf("dirpath:%s\n",dirpath);*/		
		create_msg_get_folder_meta_request(dirpath, &msg, &msg_len);
		parse_msg_get_folder_meta_response(msg, &basic_metas, &num_metas);
		if(get_header_resource(msg) != BACS_FILE || get_header_action(msg) != POST || get_header_type(msg) != BACS_RESPONSE)
    		{
			die_with_error("ls - invalid message header");
			return 1;
		}
		printf("remote directory contains: \n");
		for(i=0; i < num_metas; i++)
		{
			printf("%s\n",basic_metas[i].name);
			free(basic_metas[i].name);
		}
		free(msg);
		free(basic_metas);
		strcat(path,"/");
    		strcat(path,dir);
    		printf("Changed to directory '%s'\n", path);
	}
  return 0;
}

void list_dir(char* path, bool l)
{
	if(l==0)
{
  DIR *dp;
  struct dirent *ep;

  dp = opendir (path);
  if (dp != NULL)
  {
    while ((ep = readdir (dp)))
      puts (ep->d_name);
    closedir (dp);
  }
  else
    perror ("ls() error");
	}
	else
	{
		char* msg = 0;
		uint64_t msg_len;
		uint32_t num_metas;
		int i;
		basic_meta_t *basic_metas = 0;
		create_msg_get_folder_meta_request(path, &msg, &msg_len);
		parse_msg_get_folder_meta_response(msg, &basic_metas, &num_metas);
		if(get_header_resource(msg) != BACS_FILE || 
     	   	   get_header_action(msg) != POST ||
     	   	   get_header_type(msg) != BACS_RESPONSE)
    			die_with_error("ls - invalid message header");
		printf("remote directory contains: \n");
		for(i=0; i < num_metas; i++)
		{
			printf("%s\n",basic_metas[i].name);
			free(basic_metas[i].name);
		}
		free(msg);
		free(basic_metas);
	}
}

void make_dir(char* dir, bool l, char* path)
{
  if(l==0)
  {
    char dirpath[1024];
    int result;

    strcpy(dirpath,path);
    strcat(dirpath,"/");
    strcat(dirpath,dir);
   /* printf("dirpath:%s\n",dirpath);*/
    result = mkdir(dirpath, 0777);
    if(result==0)
      printf("Local directory '%s' created at path: %s\n", dir, path);
    else
      perror("mkdir() error");
  }
  else
  {
	char* msg = 0;
	uint64_t msg_len;
	create_msg_post_folder_request(dir, &msg, &msg_len);
	parse_msg_post_folder_response(msg);
	if(get_header_resource(msg) != BACS_FILE || get_header_action(msg) != POST || get_header_type(msg) != BACS_RESPONSE)
    			die_with_error("make_dir - invalid message header");
	free(msg);
    	
	printf("Remote directory '%s' created at path: %s\n", dir, path);
  }
}

void get_string(char *string, char *input, int index)
{
  int i=0;
  int len = strlen(input);
  while(index<len)
  {
    string[i++]=input[index++];
  }
  /* printf("%s",string); */
}

void upload(char* file_name, bool f, char* local_path, char* remote_path, char* IPaddr, int PN)
{
  if(f==0)
  {
    char filepath[1024];
    struct stat st;
	uint64_t size = st.st_size;
	char *msg = 0, *resp_msg = 0;
	uint64_t msg_len;
	uuid_t *uuids = 0;
	uint64_t num_uuids;
	ErrorCode error;
	struct Node* resp;
    printf("...Uploading file '%s'...\n", file_name);
    strcpy(filepath,local_path);
    strcat(filepath,"/");
    strcat(filepath,file_name);
	stat(filepath, &st);
    	create_msg_post_file_request(file_name, size, &msg, &msg_len);
	/*mysend(msg, ...) */
	error = mysend(msg, IPaddr, PN, msg_len);
	if(error == FAILURE || error == RETRY)
		printf("error in send");
	resp = myrecv();
	resp_msg = resp->message;
  /* Wait for response */
  /* myrecv(blah blah blah, resp_msg) */
  parse_msg_post_file_response(resp_msg, &uuids, &num_uuids);
	free(resp_msg);
  free(msg);
	if(num_uuids==0)
		printf("***************error num_uuids\n");
	if(get_header_resource(msg) != BACS_FILE || 
     	   get_header_action(msg) != POST ||
     	   get_header_type(msg) != BACS_RESPONSE)
    		die_with_error("upload_file - invalid message header");

	  send_file(filepath, IPaddr, uuids, num_uuids);
	  free(uuids);

    printf("\n...File uploaded to path: %s\n",remote_path);
  }
  else
  {
    char local_temp[1024], remote_temp[1024];
    DIR *dirp;
    struct dirent *dp; 

    strcpy(local_temp, local_path);
    strcpy(remote_temp, remote_path);
    printf("Uploading directory '%s'\n", file_name);
    make_dir(file_name, 1, remote_path);
    change_dir(file_name, remote_path, 1);
    change_dir(file_name, local_path, 0);

    /* upload individual files */
    dirp = opendir(local_path);
    if (dirp != 0) 
    {
	int success;
      while ((dp = readdir(dirp)))
      {
    
	/*upload file start*/
	struct stat st;
	uint64_t size = st.st_size;
	char *msg = 0, *resp_msg = 0;
	uint64_t msg_len;
	uuid_t *uuids = 0;
	uint64_t num_uuids;
	ErrorCode error;
	struct Node* resp;
	char filepath[1024];
        strcpy(filepath,local_path);
        strcat(filepath,"/");
        strcat(filepath,dp->d_name);
    	printf("...Uploading file '%s'...\n", filepath);
	stat(filepath, &st);
    	create_msg_post_file_request(file_name, size, &msg, &msg_len);
	error = mysend(msg, IPaddr, PN, msg_len);
	if(error == FAILURE || error == RETRY)
		printf("error in send");
	resp = myrecv();
	resp_msg = resp->message;
  	parse_msg_post_file_response(resp_msg, &uuids, &num_uuids);
  	free(msg);
	if(num_uuids==0)
		printf("***************error num_uuids\n");
	if(get_header_resource(resp_msg) != BACS_FILE || 
     	   get_header_action(resp_msg) != POST ||
     	   get_header_type(resp_msg) != BACS_RESPONSE)
    		die_with_error("upload_file - invalid message header");
	free(resp_msg);
	  send_file(filepath, IPaddr, uuids, num_uuids);
	  free(uuids);
	/*Upload file end*/
        printf("\n...File uploaded to path: %s\n",remote_path);
      }
	success = closedir(dirp);
      if(success!=0)
	perror("closdir() error");
	
    }
    strcpy(local_path, local_temp);
    strcpy(remote_path, remote_temp);
    printf("Directory uploaded to path: %s\n",remote_path);
  }
}

void download(char* file_name, bool f, char* local_path, char* remote_path, char *IPaddr, int PN)
{
  if(!f)
  {
    char filepath[1024];
	char *msg = 0;
	uint64_t msg_len;
	basic_block_t *blocks = 0;
	uint64_t num_blocks;
	int i;
	ErrorCode error;
	struct Node* resp;
	char * resp_msg = 0;
	strcpy(filepath,local_path);
    	strcat(filepath,"/");
    	strcat(filepath,file_name);
    	printf("Downloading file '%s'..\n", file_name);
	create_msg_get_file_request(filepath, &msg, &msg_len);
	error = mysend(msg, IPaddr, PN, msg_len);
	if(error == FAILURE || error == RETRY)
		printf("error in send");
	resp = myrecv();
	resp_msg = resp->message;
	parse_msg_get_file_response(resp_msg, &blocks, &num_blocks);
	if(get_header_resource(resp_msg) != BACS_FILE || get_header_action(resp_msg) != POST || get_header_type(resp_msg) != BACS_RESPONSE)
    		die_with_error("download_file - invalid message header");
	free(resp_msg);
	free(msg);
	for(i=0; i<num_blocks; i++)
	{
		char *msg = 0;
		uuid_t uuid;
		uint32_t size;
		char *content = 0;
		create_msg_get_block_request(blocks[i].uuid, &msg, &msg_len);
		parse_msg_get_block_response(msg, &uuid, &size, &content);
		free(msg);
		free(content);
	}
	free(blocks);
  /* 
	void *ans;
	FILE *fp;
 while((ans = myrecv())!=(void*)1 ||(ans = myrecv())!=(void*)2)
    {
      fputs(ans, stdout);
      printf("\n");
    }
    fp = fopen(filepath,"w");
    fwrite(ans, 1, sizeof(ans), fp);
    fclose(fp);*/
    printf("File downloaded to path: %s\n",local_path);
  }
  else
  {   
    char local_temp[1024], remote_temp[1024];

    printf("Downloading directory '%s'..\n", file_name);
    strcpy(local_temp, local_path);
    strcpy(remote_temp, remote_path);
    make_dir(file_name, 0, local_path);
    change_dir(file_name, local_path, 0);
    change_dir(file_name, remote_path, 1);

    /* download all files in dir */

    strcpy(local_path, local_temp);
    strcpy(remote_path, remote_temp);
    printf("Directory downloaded to path: %s\n",local_path);
  }
}

void delete(char* file_name, char* path, bool f, bool l)
{
	if(l==false)
	{
		if(f==false)
		{
			if(remove(file_name)==0)
				printf("File successfully deleted\n");
			else
				perror("delete() error");
		}
		else
		{
			char local_temp[1024];
    			DIR *dirp;
    			struct dirent *dp; 
			strcpy(local_temp, path);
 			if(change_dir(file_name, path, 0)==0)
			{
    				/* delete individual files */
    				dirp = opendir(path);
    				if (dirp != NULL) 
    				{
      					while ((dp = readdir(dirp)))
      					{
						if(strcmp(dp->d_name,"..")!=0 && strcmp(dp->d_name,".")!=0)
        					{
							printf("deleting file: %s\n", dp->d_name);
							if(unlink(dp->d_name)==0)
								printf("File successfully deleted\n");
							else
								perror("delete() error");
						}
      					}
      					closedir(dirp);
    				}
				else
					perror("opendir() error: ");
				
  				if(chdir(local_temp)!=0)
  					perror("cd() error");
				if(remove(file_name)==0)
					printf("Folder successfully deleted");
				else
					perror("delete() error");
            		}
			strcpy(path, local_temp);
		}
	}
	else
	{
		if(f==0)
		{
		}
		else
		{
		}
	}
}

int main(int argc, char *argv[])
{
  FILE *fp;
  int MAX_LENGTH, i, index, j;
  char IPaddr[1024];
  char *command, *input, *local_path, *remote_path;
	int PN = 9930;

  MAX_LENGTH=20;
  strcpy(IPaddr, argv[1]);
  printf("IPaddress: %s",IPaddr);
  command = (char *) malloc(sizeof(char)*MAX_LENGTH);
  input = (char *) malloc(sizeof(char)*MAX_LENGTH);
  local_path = (char *) malloc(1024);
  remote_path = (char *) malloc(1024);
  memset(local_path,0, MAX_LENGTH);
  memset(remote_path,0, MAX_LENGTH);
  /* strcpy(local_path,"/local"); */
  strcpy(remote_path,"/remote");
  strcpy(IPaddr, "127.0.0.1");
/*  if (getcwd(local_path, 1024) != NULL)
    printf("\nlocal_path: %s\n", local_path);
  else
    perror("getcwd() error");*/
strcpy(local_path,"/home/charmi/Desktop/123");
printf("\nlocal_path: %s\n", local_path);
  printf("remote_path: %s\n", remote_path);

  while(1)
  {
    memset(input,0, MAX_LENGTH);
    memset(command,0, MAX_LENGTH);
    index=0;
    printf("\n>>");
    scanf(" %[^\n]s",input);
    /* printf("%s %s","\ninput= ",input); */
    for(i=0; i<MAX_LENGTH; i++)
    {
      if(input[i]==' ')
        break;    
      command[index++]=input[i];
    }
    /* printf("%s %s","\ncommand= ",command); */
    if(strcmp(command,"help")==0)
    {
      long fsize;
      char *string;

      fp=fopen("/home/charmi/Documents/BACS/HELP","r");
      fseek(fp, 0, SEEK_END);
      fsize = ftell(fp);
      fseek(fp, 0, SEEK_SET);
      string = malloc(fsize + 1);
      fread(string, fsize, 1, fp);
      fclose(fp);
      string[fsize] = 0;
      printf("%s",string);
	free(string);
    }
    else if(strcmp(command,"upload")==0)
    {
      j=index+1;
      if(input[j]=='-' && input[j+2]==' ')
      {
        if(input[j+1]=='f')
        {
	  char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          upload(string, 0, local_path, remote_path, IPaddr, PN);
	  free(string);
        }
        else if(input[j+1]=='d')
        {
          char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          upload(string, 1, local_path, remote_path, IPaddr, PN); 
	  free(string);
        }
        else
        {
          printf("%s", "Syntax error.");
        }
      }
      else
      {
        printf("%s", "Syntax error.");
      }
    }
    else if(strcmp(command,"download")==0)
    {
      j=index+1;
      if(input[j]=='-' && input[j+2]==' ')
      {
        if(input[j+1]=='f')
        {
          char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          download(string, 0, local_path, remote_path, IPaddr, PN);
	  free(string);
        }
        else if(input[j+1]=='d')
        {
          char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          download(string, 1, local_path, remote_path, IPaddr, PN);
	  free(string);
        }
        else
        {
          printf("%s", "Syntax error.");
        }
      }
      else
      {
        printf("%s", "Syntax error.");
      }
    }
    else if(strcmp(command,"mkdir")==0)
    {
      j=index+1;
      if(input[j]=='-' && input[j+2]==' ')
      {
        if(input[j+1]=='l')
        {
          char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          make_dir(string, 0, local_path);
	  free(string);
        }
        else if(input[j+1]=='r')
        {
          char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          make_dir(string, 1, remote_path);
	  free(string);
        }
        else
        {
          printf("%s", "Syntax error.");
        }
      }
      else
      {
        printf("%s", "Syntax error.");
      }
    }
    else if(strcmp(command,"cd")==0)
    {
      j=index+1;
      if(input[j]=='-' && input[j+2]==' ')
      {
        if(input[j+1]=='l')
        {
          char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          change_dir(string, local_path, 0);
	  free(string);
        }
        else if(input[j+1]=='r')
        {
          char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          change_dir(string, remote_path, 1);
	  free(string);
        }
        else
        {
          printf("%s", "Syntax error.");
        }
      }
      else
      {
        printf("%s", "Syntax error.");
      }
    }
    else if(strcmp(command,"ls")==0)
    {
      j=index+1;
      if(input[j]=='-')
      {
        if(input[j+1]=='l')
        {
          char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          list_dir(local_path, 0);
	  free(string);
        }
        else if(input[j+1]=='r')
        {
          char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          list_dir(remote_path, 1);
	  free(string);
        }
        else
        {
          printf("%s", "Syntax error.");
        }
      }
      else
      {
        printf("%s", "Syntax error.");
      }
    }
    	else if(strcmp(command,"delete")==0)
    	{
      		j=index+1;
      		if(input[j]=='-' && input[j+2]==' ')
      		{
        		if(input[j+1]=='l')
        		{
		  		if(input[j+3]=='-' && input[j+5]==' ')
		  		{
		    			if(input[j+4]=='f')
		    			{
		      				char *string = (char*)malloc(sizeof(char)*20);
	  					memset(string,0,20);
          					get_string(string, input, j+6);
        	      				delete(string, local_path, 0, 0);
						free(string);
		    			}
		    			else if(input[j+4]=='d')
		    			{
		      				char *string = (char*)malloc(sizeof(char)*20);
	  					memset(string,0,20);
          					get_string(string, input, j+6);
        	      				delete(string, local_path, 1, 0);
						free(string);
		    			}
		    			else
        	                 		printf("%s", "Syntax error.");
        	    		}
				else
					printf("%s", "Syntax error.");
        		}
        		else if(input[j+1]=='r')
        		{
        	  		if(input[j+3]=='-' && input[j+5]==' ')
		  		{
		    			if(input[j+4]=='f')
		    			{
		      				char *string = (char*)malloc(sizeof(char)*20);
	  					memset(string,0,20);
          					get_string(string, input, j+6);
        	      				delete(string, remote_path, 0, 1);
						free(string);
		    			}
		    			else if(input[j+4]=='d')
		    			{
		      				char *string = (char*)malloc(sizeof(char)*20);
	  					memset(string,0,20);
          					get_string(string, input, j+6);
        	      				delete(string, remote_path, 1, 1);
						free(string);
		    			}
		    			else
        	      				printf("%s", "Syntax error.");
        	    		}
        			else
        	  			printf("%s", "Syntax error.");
			}
			else
				printf("%s", "Syntax error.");
        	}
      		else
      		        printf("%s", "Syntax error.");
	}
    else if(strcmp(command,"exit")==0)
    {
	free(command); 
  	free(input); 
  	free(local_path); 
  	free(remote_path); 
      	exit(EXIT_SUCCESS);
    }
  }
  return(0);
}
