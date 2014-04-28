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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "common.h"


bool change_dir(char* dir, char* path, bool l, unsigned long IPaddr, int PN)
{
	if(l==0)
	{
  		char dirpath[1024];
  		int result;
		memset(dirpath, 0, 1024);
		if(strcmp(path,"/")==0 && strcmp(dir,"..")==0)
			strcpy(dirpath,path);
		else if(strcmp(dir,"..")!=0 && strcmp(path,"/")!=0)
		{
			strcpy(dirpath,path);
  			strcat(dirpath,"/");
  			strcat(dirpath,dir);
		}
		else if(strcmp(dir,"..")!=0 && strcmp(path,"/")==0)
		{
			strcpy(dirpath,path);
  			strcat(dirpath,dir);
		}
		else if(strcmp(path,"/")!=0)
		{
    			char** tokens;
    			tokens = str_split(path, '/');
			if (tokens)
    			{
        			int i;
        			for (i = 0; *(tokens + i); i++);
				if(i!=1)
				{
					int j;
					for (j = 0; j<i-1; j++)
        				{
						strcat(dirpath,"/");
						strcat(dirpath,*(tokens + j));
            					free(*(tokens + j));
        				}
				}
				else
					strcpy(dirpath,"/");
        			free(tokens);
			}
		}
		else
			strcpy(dirpath,"/");
  		result = chdir(dirpath);
  		if(result==0)
  		{
    			memset(path, 0, 1024);
			strcpy(path, dirpath);
    			printf("Changed to local directory '%s'\n", path);
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
		ErrorCode error;
		char* resp_msg =0, *err_msg_string = 0;
		struct Node* resp;
  		memset(dirpath, 0, 1024);
		if(strcmp(path,"/")==0 && strcmp(dir,"..")==0)
			strcpy(dirpath, path);
		else if(strcmp(dir,"..")!=0 && strcmp(path,"/")!=0)
		{
			strcpy(dirpath,path);
  			strcat(dirpath,"/");
  			strcat(dirpath,dir);
		}
		else if(strcmp(dir,"..")!=0 && strcmp(path,"/")==0)
		{
			strcpy(dirpath,path);
  			strcat(dirpath,dir);
		}
		else if(strcmp(path,"/")!=0)
		{
    			char** tokens;
    			tokens = str_split(path, '/');
			if (tokens)
    			{
        			int i;
        			for (i = 0; *(tokens + i); i++);
				if(i!=1)
				{
					int j;
					for (j = 0; j<i-1; j++)
        				{
						strcat(dirpath,"/");
						strcat(dirpath,*(tokens + j));
            					free(*(tokens + j));
        				}
				}
				else
					strcpy(dirpath,"/");
        			free(tokens);
			}
		}
		else
			strcpy(dirpath,"/");
  		printf("dirpath:%s\n",dirpath);		
		create_msg_get_folder_meta_request(dirpath, &msg, &msg_len);
		error = mysend(msg, IPaddr, PN, msg_len);
		if(error == FAILURE || error == RETRY)
			printf("error in send\n");
		resp = myrecv(CLIENT_PORT);
		resp_msg = resp->message;
		if(get_header_type(resp_msg) == BACS_ERROR)
		{
      			parse_msg_error(resp_msg, &err_msg_string);
			printf("%s\n",err_msg_string);
		}
    		else
		{
			parse_msg_get_folder_meta_response(resp_msg, &basic_metas, &num_metas);
			printf("remote directory contains: \n");
			for(i=0; i < num_metas; i++)
			{
				printf("%s\n",basic_metas[i].name);
				free(basic_metas[i].name);
			}
			memset(path, 0, 1024);
			strcpy(path, dirpath);
		}
		free(err_msg_string);
		free(msg);
		free(resp->message);
		free(resp);
		free(basic_metas);
		printf("Remote directory '%s'\n", path);
	}
  return 0;
}

void list_dir(char* path, bool l, unsigned long IPaddr, int PN)
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
		char* msg = 0, * resp_msg = 0, *err_msg_string = 0;
		uint64_t msg_len;
		uint32_t num_metas;
		int i;
		basic_meta_t *basic_metas = 0;
		ErrorCode error;
		struct Node* resp;
		create_msg_get_folder_meta_request(path, &msg, &msg_len);
		error = mysend(msg, IPaddr, PN, msg_len);
		if(error == FAILURE || error == RETRY)
			printf("error in send\n");
		resp = myrecv(CLIENT_PORT);
		resp_msg = resp->message;
		if(get_header_type(resp_msg) == BACS_ERROR)
		{
      			parse_msg_error(resp_msg, &err_msg_string);
			printf("%s\n",err_msg_string);
		}
    		else
		{
			parse_msg_get_folder_meta_response(resp_msg, &basic_metas, &num_metas);		
			printf("remote directory contains: \n");
			for(i=0; i < num_metas; i++)
			{
				printf("%s\n",basic_metas[i].name);
				free(basic_metas[i].name);
			}
		}
		free(err_msg_string);
		free(msg);
		free(resp->message);
		free(resp);
		free(basic_metas);
	}
}

void make_dir(char* dir, bool l, char* path, unsigned long IPaddr, int PN)
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
	char* msg = 0, * resp_msg = 0, *err_msg_string = 0;
	uint64_t msg_len;
	ErrorCode error;
	struct Node* resp;
	char dirpath[1024];
    	strcpy(dirpath,path);
	if(strcmp(path,"/")!=0)
    		strcat(dirpath,"/");
    	strcat(dirpath,dir);
	create_msg_post_folder_request(dirpath, &msg, &msg_len);
	error = mysend(msg, IPaddr, PN, msg_len);
	if(error == FAILURE || error == RETRY)
			printf("error in send\n");
	resp = myrecv(CLIENT_PORT);
	resp_msg = resp->message;
	if(get_header_type(resp_msg) == BACS_ERROR)
	{
      		parse_msg_error(resp_msg, &err_msg_string);
		printf("%s\n",err_msg_string);
	}
    	else
	{
		parse_msg_post_folder_response(resp_msg);
		printf("Remote directory '%s' created at path: %s\n", dir, path);
	}
	free(err_msg_string);
	free(msg);
	free(resp->message);
	free(resp);
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

bool upload_file(char* file_name, char* local_path, char* remote_path, unsigned long IPaddr, int PN)
{
    	char filepath[1024];
 	struct stat st;
	uint64_t size;
	char *msg = 0, *resp_msg = 0, *err_msg_string = 0;
	uint64_t msg_len;
	uuid_t *uuids = 0;
	uint64_t num_uuids;
	ErrorCode error;
	struct Node* resp;
    	printf("...Uploading file '%s'...\n", file_name);
   	strcpy(filepath,local_path);
	strcat(filepath,"/");
    	strcat(filepath,file_name);
	if(stat(filepath, &st)==0)
	{
		size = st.st_size;
		printf("size= %d\n", size);
		if(size == 0)
		{
			printf("File empty. Cannot be uploaded\n");
			return 0;
		}
	}
	else
	{	
		perror("size error: ");
		return 0;
	}
	memset(filepath, 0, 1024);
	strcpy(filepath,remote_path);
	if(strcmp(remote_path,"/")!=0)
		strcat(filepath,"/");
    	strcat(filepath,file_name);
    	create_msg_post_file_request(filepath, size, &msg, &msg_len);
	/*mysend(msg, ...) */
	error = mysend(msg, IPaddr, PN, msg_len);
	if(error == FAILURE || error == RETRY)
	{
		printf("error in send\n");
		free(msg);
		return 0;
	}
	else		
	{
		resp = myrecv(CLIENT_PORT);
		resp_msg = resp->message;
		if(get_header_type(resp_msg) == BACS_ERROR)
		{
      			parse_msg_error(resp_msg, &err_msg_string);
			printf("%s\n",err_msg_string);
			free(err_msg_string);
			free(resp->message);
			free(resp);
  			free(msg);
			return 0;
		}
    		else
  		{
			parse_msg_post_file_response(resp_msg, &uuids, &num_uuids);
			if(num_uuids==0)
			{
				printf("***************error num_uuids\n");
				free(err_msg_string);
				free(resp->message);
				free(resp);
  				free(msg);
				free(uuids);
				return 0;
			}
			else
			{
				memset(filepath, 0, 1024);
				strcpy(filepath,local_path);
				strcat(filepath,"/");
    				strcat(filepath,file_name);
				bool success = send_file(filepath, IPaddr, uuids, num_uuids, PN);
				if(success == 0)
				{
					free(err_msg_string);
					free(resp->message);
					free(resp);
  					free(msg);
					free(uuids);
					return 0;
				}
			}
		}	
	}
	free(err_msg_string);
	free(resp->message);
	free(resp);
  	free(msg);
	free(uuids);
	return 1;
}	


void upload_folder(char* file_name, char* local_path, char* remote_path, unsigned long IPaddr, int PN)
{
    	char local_temp[1024], remote_temp[1024];
    	DIR *dirp;
    	struct dirent *dp; 
	int empty = 1;
	strcpy(local_temp, local_path);
    	strcpy(remote_temp, remote_path);
    	change_dir(file_name, local_path, 0, IPaddr, PN);
	if(strcmp(remote_path, "/")!=0)
		strcat(remote_path,"/");
	strcat(remote_path, file_name);
	/* upload individual files */
    	dirp = opendir(local_path);
    	if (dirp != 0) 
    	{
		int success;
      		while ((dp = readdir(dirp)))
      		{
			if(strcmp(dp->d_name,".")!=0 && strcmp(dp->d_name,"..")!=0)
			{
    				bool success = upload_file(dp->d_name, local_path, remote_path, IPaddr, PN);
				if(success == 0)
				{
					printf("File '%s' not uploaded successfully\n",dp->d_name);
				}
				empty = 0;
			}
      		}
		success = closedir(dirp);
      		if(success!=0)
			perror("closdir() error");
	}
    	strcpy(local_path, local_temp);
    	strcpy(remote_path, remote_temp);
	if(empty)
	{
		make_dir(file_name, 1, remote_temp, IPaddr, PN);
	}
}

void download_file(char* file_name, char* local_path, char* remote_path, unsigned long IPaddr, int PN)
{
	FILE *fp;   	
	char local_file[1024];
	char remote_file[1024];
	char *msg = 0;
	uint64_t msg_len;
	basic_block_t *blocks = 0;
	uint64_t num_blocks;
	int i;
	ErrorCode error;
	struct Node* resp;
	char * resp_msg = 0, *err_msg_string = 0;
	strcpy(local_file,local_path);
    	strcat(local_file,"/");
    	strcat(local_file,file_name);
	strcpy(remote_file,remote_path);
	if(strcmp(remote_path,"/")!=0)
    		strcat(remote_file,"/");
    	strcat(remote_file,file_name);
    	printf("Downloading file '%s'..\n", file_name);
	create_msg_get_file_request(remote_file, &msg, &msg_len);
	error = mysend(msg, IPaddr, PN, msg_len);
	if(error == FAILURE || error == RETRY)
		printf("error in send\n");
	else
	{
		resp = myrecv(CLIENT_PORT);
		resp_msg = resp->message;
		if(get_header_type(resp_msg) == BACS_ERROR)
		{
      			parse_msg_error(resp_msg, &err_msg_string);
			printf("%s\n",err_msg_string);
		}
    		else
		{
			bool flag = 0;
			parse_msg_get_file_response(resp_msg, &blocks, &num_blocks);
			fp = fopen(local_file, "w"); 
			for(i=0; i<num_blocks; i++)
			{
				char *msg = 0, *resp_msg = 0, *err_msg_string = 0;
				uuid_t uuid;
				uint32_t size;
				char *content = 0;
				ErrorCode error;
				struct  Node* resp;
				create_msg_get_block_request(blocks[i].uuid, &msg, &msg_len);
				error = mysend(msg, IPaddr, PN, msg_len);
				if(error == FAILURE || error == RETRY)
				{
					printf("error in send\n");
					flag = 1;
				}
				else
				{
					resp = myrecv(CLIENT_PORT);
					resp_msg = resp->message;
					if(get_header_type(resp_msg) == BACS_ERROR)
					{
	      					parse_msg_error(resp_msg, &err_msg_string);
						printf("%s\n",err_msg_string);
						flag =1;
					}
	    				else
					{	
						parse_msg_get_block_response(resp_msg, &uuid, &size, &content);
						fwrite (content, sizeof(char), size, fp);
					}
				}
				free(err_msg_string);
				free(msg);
				free(resp->message);
				free(resp);
				free(content);
			}	
			if(!flag)
    				printf("File downloaded to path: %s\n",local_path);
			fclose(fp);
		}
	}
	free(err_msg_string);
	free(resp->message);
	free(resp);
	free(msg);
	free(blocks);
}

void download_folder(char* file_name, char* local_path, char* remote_path, unsigned long IPaddr, int PN)
{
	char local_temp[1024], remote_temp[1024];
	char* msg = 0, * resp_msg = 0, *err_msg_string = 0;
	uint64_t msg_len;
	uint32_t num_metas;
	int i;
	basic_meta_t *basic_metas = 0;
	ErrorCode error;
	struct Node* resp;

	printf("Downloading directory '%s'..\n", file_name);
    strcpy(local_temp, local_path);
    strcpy(remote_temp, remote_path);
    make_dir(file_name, 0, local_path, IPaddr, PN);
    change_dir(file_name, local_temp, 0, IPaddr, PN);
    change_dir(file_name, remote_temp, 1, IPaddr, PN);

	create_msg_get_folder_meta_request(remote_temp, &msg, &msg_len);
	error = mysend(msg, IPaddr, PN, msg_len);
	if(error == FAILURE || error == RETRY)
		printf("error in send\n");
	resp = myrecv(CLIENT_PORT);
	resp_msg = resp->message;
	if(get_header_type(resp_msg) == BACS_ERROR)
	{
      		parse_msg_error(resp_msg, &err_msg_string);
		printf("%s",err_msg_string);
	}
    	else
		parse_msg_get_folder_meta_response(resp_msg, &basic_metas, &num_metas);
	free(err_msg_string);
	printf("remote directory contains: \n");
	for(i=0; i < num_metas; i++)
	{
		FILE *fp;   	
		char local_file[1024];
		char remote_file[1024];
		char *msg = 0;
		uint64_t msg_len;
		basic_block_t *blocks = 0;
		uint64_t num_blocks;
		int i;
		ErrorCode error;
		struct Node* resp;
		char * resp_msg = 0, *err_msg_string = 0;

		printf("%s\n",basic_metas[i].name);
		strcpy(local_file,local_temp);
	    	strcat(local_file,"/");
	    	strcat(local_file,basic_metas[i].name);
		strcpy(remote_file,remote_temp);
	    	strcat(remote_file,"/");
	    	strcat(remote_file,basic_metas[i].name);
		fp = fopen(local_file, "w"); 
	    	printf("Downloading file '%s'..\n", file_name);
		create_msg_get_file_request(remote_file, &msg, &msg_len);
		error = mysend(msg, IPaddr, PN, msg_len);
		if(error == FAILURE || error == RETRY)
			printf("error in send\n");
		resp = myrecv(CLIENT_PORT);
		resp_msg = resp->message;
		if(get_header_type(resp_msg) == BACS_ERROR)
		{
      			parse_msg_error(resp_msg, &err_msg_string);
			printf("%s\n",err_msg_string);
		}
    		else
			parse_msg_get_file_response(resp_msg, &blocks, &num_blocks);
		free(err_msg_string);
		free(resp->message);
		free(resp);
		free(msg);
		for(i=0; i<num_blocks; i++)
		{
			char *msg = 0, *resp_msg = 0, *err_msg_string = 0;
			uuid_t uuid;
			uint32_t size;
			char *content = 0;
			ErrorCode error;
			struct  Node* resp;
			create_msg_get_block_request(blocks[i].uuid, &msg, &msg_len);
			error = mysend(msg, IPaddr, PN, msg_len);
			if(error == FAILURE || error == RETRY)
				printf("error in send\n");
			resp = myrecv(CLIENT_PORT);
			resp_msg = resp->message;
			if(get_header_type(resp_msg) == BACS_ERROR)
			{
      				parse_msg_error(resp_msg, &err_msg_string);
			printf("%s\n",err_msg_string);
			}
    			else			
				parse_msg_get_block_response(resp_msg, &uuid, &size, &content);
			free(err_msg_string);
			fwrite (content, sizeof(char), size, fp);
			free(msg);
			free(resp->message);
			free(resp);
			free(content);
		}
		free(blocks);
		fclose(fp);
	    	printf("File downloaded to path: %s\n",local_temp);
		free(basic_metas[i].name);
	}
	free(msg);
	free(resp->message);
	free(resp);
	free(basic_metas);
 	/*download file complete*/
   
    strcpy(local_path, local_temp);
    strcpy(remote_path, remote_temp);
    printf("Directory downloaded to path: %s\n",local_path);
}

void delete(char* file_name, char* path, bool f, bool l, unsigned long IPaddr, int PN)
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
 			if(change_dir(file_name, path, 0, IPaddr, PN)==0)
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
					printf("Folder successfully deleted\n");
				else
					perror("delete() error");
            		}
			strcpy(path, local_temp);
		}
	}
	else
	{
		char filepath[1024];
		strcpy(filepath, path);
		strcat(filepath, "/");
		strcat(filepath, file_name);
		if(f==0)
		{
			char* msg = 0, * resp_msg = 0, *err_msg_string = 0;
			uint64_t msg_len;
			ErrorCode error;
			struct Node* resp;
			create_msg_delete_file_request(filepath, &msg, &msg_len);
			error = mysend(msg, IPaddr, PN, msg_len);
			if(error == FAILURE || error == RETRY)
				printf("error in send\n");
			resp = myrecv(CLIENT_PORT);
			resp_msg = resp->message;
			if(get_header_type(resp_msg) == BACS_ERROR)
			{
      				parse_msg_error(resp_msg, &err_msg_string);
				printf("%s\n",err_msg_string);
			}
    			else
				parse_msg_delete_file_response(resp_msg);
			free(err_msg_string);
			free(msg);
			free(resp->message);
			free(resp);
    	
			printf("Deleted remote file '%s'\n", file_name);
		}
		else
		{
			char* msg = 0, * resp_msg = 0, *err_msg_string = 0;
			uint64_t msg_len;
			ErrorCode error;
			struct Node* resp;
			create_msg_delete_folder_request(filepath, &msg, &msg_len);
			error = mysend(msg, IPaddr, PN, msg_len);
			if(error == FAILURE || error == RETRY)
				printf("error in send\n");
			resp = myrecv(CLIENT_PORT);
			resp_msg = resp->message;
			if(get_header_type(resp_msg) == BACS_ERROR)
			{
      				parse_msg_error(resp_msg, &err_msg_string);
				printf("%s\n",err_msg_string);
			}
    			else
				parse_msg_delete_folder_response(resp_msg);
			free(err_msg_string);
			free(msg);
			free(resp->message);
			free(resp);
    	
			printf("Deleted remote folder '%s'\n", file_name);
		}
	}
}

int main(int argc, char *argv[])
{
  FILE *fp;
  int MAX_LENGTH, i, index, j;
  char *command, *input, *local_path, *remote_path;
  int PN = 0;
  struct in_addr IP;
  unsigned long IPaddr = 0;
  MAX_LENGTH=20;
  if (inet_aton(argv[1], &IP)==0)
  {
   	printf("Unable to convert IPaddr to int\n");
  }
  else
	IPaddr = IP.s_addr;
  PN = atoi(argv[2]);
  printf("IPaddress: %lu\n",IPaddr);
  printf("PN: %d\n", PN);
  socket_send_create(PN);
  socket_receive_create(CLIENT_PORT);
  command = (char *) malloc(sizeof(char)*MAX_LENGTH);
  input = (char *) malloc(sizeof(char)*MAX_LENGTH);
  local_path = (char *) malloc(1024);
  remote_path = (char *) malloc(1024);
  memset(local_path,0, MAX_LENGTH);
  memset(remote_path,0, MAX_LENGTH);
  /* strcpy(local_path,"/local"); */
  strcpy(remote_path,"/");
 if (getcwd(local_path, 1024) != NULL)
    printf("\nlocal_path: %s\n", local_path);
  else
    perror("getcwd() error");
/*strcpy(local_path,"/home/charmi/Desktop/123");*/
/*printf("\nlocal_path: %s\n", local_path);*/
  printf("remote_path: '%s'\n", remote_path);

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
	char filepath[1024];
	strcpy(filepath, local_path);
	strcpy(filepath, "/");
	strcpy(filepath, "HELP");
      fp=fopen(filepath,"r");
      fseek(fp, 0, SEEK_END);
      fsize = ftell(fp);
      fseek(fp, 0, SEEK_SET);
      string = malloc(fsize + 1);
      fread(string, fsize, 1, fp);
      fclose(fp);
      string[fsize] = 0;
      printf("%s\n",string);
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
          upload_file(string, local_path, remote_path, IPaddr, PN);
	  free(string);
        }
        else if(input[j+1]=='d')
        {
          char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          upload_folder(string, local_path, remote_path, IPaddr, PN); 
	  free(string);
        }
        else
        {
          printf("%s\n", "Syntax error.");
        }
      }
      else
      {
        printf("%s\n", "Syntax error.");
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
          download_file(string, local_path, remote_path, IPaddr, PN);
	  free(string);
        }
        else if(input[j+1]=='d')
        {
          char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          download_folder(string, local_path, remote_path, IPaddr, PN);
	  free(string);
        }
        else
        {
          printf("%s\n", "Syntax error.");
        }
      }
      else
      {
        printf("%s\n", "Syntax error.");
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
          make_dir(string, 0, local_path, IPaddr, PN);
	  free(string);
        }
        else if(input[j+1]=='r')
        {
          char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          make_dir(string, 1, remote_path, IPaddr, PN);
	  free(string);
        }
        else
        {
          printf("%s\n", "Syntax error.");
        }
      }
      else
      {
        printf("%s\n", "Syntax error.");
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
          change_dir(string, local_path, 0, IPaddr, PN);
	  free(string);
        }
        else if(input[j+1]=='r')
        {
          char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          change_dir(string, remote_path, 1, IPaddr, PN);
	  free(string);
        }
        else
        {
          printf("%s\n", "Syntax error.");
        }
      }
      else
      {
        printf("%s\n", "Syntax error.");
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
          list_dir(local_path, 0, IPaddr, PN);
	  free(string);
        }
        else if(input[j+1]=='r')
        {
          char *string = (char*)malloc(sizeof(char)*20);
	  memset(string,0,20);
          get_string(string, input, j+3);
          list_dir(remote_path, 1, IPaddr, PN);
	  free(string);
        }
        else
        {
          printf("%s\n", "Syntax error.");
        }
      }
      else
      {
        printf("%s\n", "Syntax error.");
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
        	      				delete(string, local_path, 0, 0, IPaddr, PN);
						free(string);
		    			}
		    			else if(input[j+4]=='d')
		    			{
		      				char *string = (char*)malloc(sizeof(char)*20);
	  					memset(string,0,20);
          					get_string(string, input, j+6);
        	      				delete(string, local_path, 1, 0, IPaddr, PN);
						free(string);
		    			}
		    			else
        	                 		printf("%s\n", "Syntax error.");
        	    		}
				else
					printf("%s\n", "Syntax error.");
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
        	      				delete(string, remote_path, 0, 1, IPaddr, PN);
						free(string);
		    			}
		    			else if(input[j+4]=='d')
		    			{
		      				char *string = (char*)malloc(sizeof(char)*20);
	  					memset(string,0,20);
          					get_string(string, input, j+6);
        	      				delete(string, remote_path, 1, 1, IPaddr, PN);
						free(string);
		    			}
		    			else
        	      				printf("%s\n", "Syntax error.");
        	    		}
        			else
        	  			printf("%s\n", "Syntax error.");
			}
			else
				printf("%s\n", "Syntax error.");
        	}
      		else
      		        printf("%s\n", "Syntax error.");
	}
    else if(strcmp(command,"exit")==0)
    {
	socket_send_close();
  	socket_receive_close();
	free(command); 
  	free(input); 
  	free(local_path); 
  	free(remote_path); 
      	exit(EXIT_SUCCESS);
    }
  }
  return(0);
}
