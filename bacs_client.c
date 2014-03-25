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

#define MAX_LENGTH 10
#define FILE 0
#define DIR 1

upload(char* file_name, bool f, char* local_path, char* remote_path);
download(char* file_name, bool f, char* local_path, char* remote_path);
change_local_dir(char* path, char* local_path);
change_remote_dir(char* path, char* remote_path);
char* get_string(char *input, int index)
{
	int i=0;
	int len = strlen(input);
	char *string = malloc(sizeof(char)*MAX_LENGTH);
	int k, name_index=0;
	while(index<len)
	{
		string[i++]=input[index++];
	}
	printf("%s",string);
	return string;
}

int main(int argc, char **argv)
{
	//FILE *fp;
	int i, index, j;
	char *command = (char *) malloc(sizeof(char)*MAX_LENGTH);
  	char *input = (char *) malloc(sizeof(char)*MAX_LENGTH);
	char *local_path = (char *) malloc(sizeof(char)*MAX_LENGTH);
	char *remote_path = (char *) malloc(sizeof(char)*MAX_LENGTH);
	while(1)
	{
		memset(input,0, MAX_LENGTH);
		memset(command,0, MAX_LENGTH);
		index=0;
		printf("%s","\n>>");
		scanf(" %[^\n]s",input);
	//	printf("%s %s","\ninput= ",input);
		for(i=0; i<MAX_LENGTH; i++)
		{
			if(input[i]==' ')
				break;		
			command[index++]=input[i];
		}
	//	printf("%s %s","\ncommand= ",command);
		if(strcmp(command,"help")==0)
		{
		/*	fp=fopen("/home/charmi/Documents/HELP","r");
			fseek(fp, 0, SEEK_END);
			long fsize = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			char *string = malloc(fsize + 1);
			fread(string, fsize, 1, fp);
			fclose(fp);
			string[fsize] = 0;
			printf("%s",string);*/
		}
		else if(strcmp(command,"upload")==0)
		{
			j=index+1;
			if(input[j]=='-' && input[j+2]==' ')
			{
				if(input[j+1]=='f')
				{
					char* string = get_string(input, j+3);
					//upload(string, FILE, local_path, remote_path);
				}
				else if(input[j+1]=='d')
				{
					char* string = get_string(input, j+3);
					//upload(string, DIR, local_path, remote_path);	
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
					//download(string, FILE, local_path, remote_path);
				}
				else if(input[j+1]=='d')
				{
					char* string = get_string(input, j+3);
					//download(string, DIR, local_path, remote_path);
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
				//	change_local_dir(string, local_path);
				}
				else if(input[j+1]=='r')
				{
					char* string = get_string(input, j+3);
				//	change_remote_dir(string, local_path);
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
		}
		else if(strcmp(command,"exit")==0)
		{
			exit(EXIT_SUCCESS);
		}
	}
}
