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
#include "send_file.h"

void change_dir(char* dir, char* path)
{
  char dirpath[1024];
  int result;

  strcpy(dirpath,path);
  strcat(dirpath,"/");
  strcat(dirpath,dir);
  printf("dirpath:%s\n",dirpath);
  result = chdir(dirpath);
  if(result==0)
  {
    strcat(path,"/");
    strcat(path,dir);
    printf("Changed to directory '%s'\n", path);
  }
  else 
    perror("mkdir() error");
}

void list_dir(char* path)
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

void make_dir(char* dir, bool l, char* path)
{
  if(l==0)
  {
    char dirpath[1024];
    int result;

    strcpy(dirpath,path);
    strcat(dirpath,"/");
    strcat(dirpath,dir);
    printf("dirpath:%s\n",dirpath);
    result = mkdir(dirpath, 0777);
    if(result==0)
      printf("Local directory '%s' created at path: %s\n", dir, path);
    else
      perror("mkdir() error");
  }
  else
  {
    printf("Remote directory '%s' created at path: %s\n", dir, path);
  }
}

char* get_string(char *input, int index)
{
  int i=0;
  int len = strlen(input);
  char *string = (char*)malloc(sizeof(char)*20);
  int k, name_index=0;

  memset(string,0,20);
  while(index<len)
  {
    string[i++]=input[index++];
  }
  /* printf("%s",string); */
  return string;
}

void upload(char* file_name, bool f, char* local_path, char* remote_path, char* IPaddr)
{
  if(f==0)
  {
    char filepath[1024];
    
    printf("...Uploading file '%s'...\n", file_name);
    strcpy(filepath,local_path);
    strcat(filepath,"/");
    strcat(filepath,file_name);
    send_file(filepath, IPaddr);
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
    change_dir(file_name, remote_path);
    change_dir(file_name, local_path);

    /* upload individual files */
    dirp = opendir(local_path);
    while (dirp) 
    {
      if ((dp = readdir(dirp)) != NULL) 
      {
        char filepath[1024];
        strcpy(filepath,local_path);
        strcat(filepath,"/");
        strcat(filepath,dp->d_name);
        send_file(filepath, IPaddr);
        printf("\n...File uploaded to path: %s\n",remote_path);
      }
      closedir(dirp);
    }
            
    strcpy(local_path, local_temp);
    strcpy(remote_path, remote_temp);
    printf("Directory uploaded to path: %s\n",remote_path);
  }
}

void download(char* file_name, bool f, char* local_path, char* remote_path)
{
  if(!f)
  {
    /*struct Send_message ans;
    char filepath[1024];
    FILE *fp;

    printf("Downloading file '%s'..\n", file_name);
    while((*ans = myrecv())!=(void*)1 || (*ans = myrecv())!=(void*)2)
    {
      fputs(ans, stdout);
      printf("\n");
    }

    strcpy(filepath,local_path);
    strcat(filepath,"/");
    strcat(filepath,file_name);
    fp = fopen(filepath,"w");
    fwrite(ans, 1, sizeof(ans), fp);
    fclose(fp);
    printf("File downloaded to path: %s\n",local_path);*/
  }
  else
  {   
    char local_temp[1024], remote_temp[1024];

    printf("Downloading directory '%s'..\n", file_name);
    strcpy(local_temp, local_path);
    strcpy(remote_temp, remote_path);
    make_dir(file_name, 0, local_path);
    change_dir(file_name, local_path);
    change_dir(file_name, remote_path);

    /* download all files in dir */

    strcpy(local_path, local_temp);
    strcpy(remote_path, remote_temp);
    printf("Directory downloaded to path: %s\n",local_path);
  }
}

int main(int argc, char **argv)
{
  FILE *fp;
  int MAX_LENGTH, i, index, j;
  char IPaddr[1024];
  char *command, *input, *local_path, *remote_path;

  MAX_LENGTH=20;
  command = (char *) malloc(sizeof(char)*MAX_LENGTH);
  input = (char *) malloc(sizeof(char)*MAX_LENGTH);
  local_path = (char *) malloc(1024);
  remote_path = (char *) malloc(1024);
  memset(local_path,0, MAX_LENGTH);
  memset(remote_path,0, MAX_LENGTH);
  /* strcpy(local_path,"/local"); */
  strcpy(remote_path,"/remote");
  strcpy(IPaddr, "127.0.0.1");
  if (getcwd(local_path, 1024) != NULL)
    printf("\nlocal_path: %s\n", local_path);
  else
    perror("getcwd() error");
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
    }
    else if(strcmp(command,"upload")==0)
    {
      j=index+1;
      if(input[j]=='-' && input[j+2]==' ')
      {
        if(input[j+1]=='f')
        {
          char* string = get_string(input, j+3);
          upload(string, 0, local_path, remote_path, IPaddr);
        }
        else if(input[j+1]=='d')
        {
          char* string = get_string(input, j+3);
          upload(string, 1, local_path, remote_path, IPaddr); 
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
          char* string = get_string(input, j+3);
          download(string, 0, local_path, remote_path);
        }
        else if(input[j+1]=='d')
        {
          char* string = get_string(input, j+3);
          download(string, 1, local_path, remote_path);
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
          char* string = get_string(input, j+3);
          make_dir(string, 0, local_path);
        }
        else if(input[j+1]=='r')
        {
          char* string = get_string(input, j+3);
          make_dir(string, 1, remote_path);
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
          char* string = get_string(input, j+3);
          change_dir(string, local_path);
        }
        else if(input[j+1]=='r')
        {
          char* string = get_string(input, j+3);
          change_dir(string, remote_path);
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
          char* string = get_string(input, j+3);
          list_dir(local_path);
        }
        else if(input[j+1]=='r')
        {
          char* string = get_string(input, j+3);
          list_dir(remote_path);
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
    else if(strcmp(command,"exit")==0)
    {
      exit(EXIT_SUCCESS);
    }
  }
}
