#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h> 
#include "definitions.h"
#include <string.h>
#include <stdbool.h>
#include "UDPserver.h"

/*void err(char *str)
{
    perror(str);
    exit(1);
}*/
/*create_new_node(head,recv_message.seq_number, cli_addr.sin_addr, recv_message.port_number,recv_message.size_of_blocks)*/
void create_new_node(struct Node* prev_node, int seq_number, long IP, int PN, int n,int total_blocks){
	struct Node* node = malloc(sizeof(struct Node)) ;
	node->next = NULL;
	if(prev_node == NULL) 
		head = node;
	if(prev_node!=NULL)
		prev_node->next = node;
	node->previous = prev_node;
	node->message = malloc(sizeof(char)*n);
	memset(node->message, 0, sizeof(char)*n);
	node->IP = IP;
	node->PN = PN;
	node->size_of_blocks=n;
	node->counter = total_blocks;
	return;
}

void delete_node(struct Node* remove_current_node_from_list){
	if(remove_current_node_from_list->previous!=NULL)
	remove_current_node_from_list->previous->next = remove_current_node_from_list->next;
	if(remove_current_node_from_list->next!=NULL)
	remove_current_node_from_list->next->previous = remove_current_node_from_list->previous;
}

struct Node* myrecv(int PN)
{	
	
	struct sockaddr_in my_addr;
	struct Send_message recv_message;
	int sockfd; 
	int slen=sizeof(my_addr);
	//socklen_t slen=sizeof(my_addr);
	char ackbuf[BUFLEN];
	struct Node* Itr = head;
	/*For copying and concatenating a string block*/
	/*char *temp;*/
			
	
	printf("Inside myrecv\n");
	
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	{
		printf("Error: Socket Not Created\n");
		return 0; /* (void*)FAILURE; */
	}	
	
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	/*
	if(PN==NULL){
		my_addr.sin_port = htons(PORT);
	}
	else*/
		my_addr.sin_port = PN;
	printf("Port: %d\n",PN);
	//my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr* ) &my_addr, sizeof(my_addr))==-1)		
	{	
		printf("Error: Bind Failure\n");
		return 0; /* (void*)FAILURE; */
	}	
	/*memset(voidbuf, 0, sizeof(voidbuf));*/
	while(1){
		printf("\n");
		memset((void*)&recv_message,0,sizeof(recv_message));
		if (recvfrom(sockfd, &recv_message, sizeof(recv_message), 0, (struct sockaddr*)&my_addr, &slen)==-1)
		{	    	
			printf("Error: Recvfrom Failure\n");
			return 0;/* (void*)FAILURE; */
		}
		
		printf("Message received: ");
		
		print_msg(recv_message.message);
		printf("\n\n");
		/*printf("Number of Blocks to be received %d\n", recv_message.seq_number);*/
	
		/*printf("Received from client %s:%d:\t:%s\n\n",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port), voidbuf);*/
		memset(ackbuf, 0, sizeof(ackbuf));
		strcpy(ackbuf,"ACK");	
		
		if (sendto(sockfd, ackbuf, BUFLEN, 0, (struct sockaddr*)&my_addr, slen)==-1)
		{
			printf("Error: Sendto Failure\n");
			return 0;/* (void*)FAILURE; */
		}
		printf("ACK SENT\n");
		if(recv_message.seq_number==0){
		printf("seq number = 0\n");
			if(head == NULL){
				int total_blocks = ceil((float)recv_message.size_of_blocks/BUFLEN);
				printf("head is null\n");
				create_new_node(head,recv_message.seq_number, my_addr.sin_addr.s_addr, recv_message.port_number,recv_message.size_of_blocks,total_blocks);
				}
			else{
			int total_blocks = ceil((float)recv_message.size_of_blocks/BUFLEN);
			printf("head not null\n");
			while(Itr->next!=NULL){  		
				Itr = Itr->next;
			}
			    
			    create_new_node(Itr,recv_message.seq_number, my_addr.sin_addr.s_addr, recv_message.port_number,recv_message.size_of_blocks,total_blocks);
			}
		}
		
			
		else{
			printf("\n\n");
			Itr = head;
			while((Itr->IP != my_addr.sin_addr.s_addr)&&(Itr->next!=NULL)){
				Itr = Itr->next;
			}
			//char *temp;
			//temp = malloc(sizeof(char)*recv_message.size_of_blocks);
			printf("recv msg: ");
			print_msg(recv_message.message);
			memcpy(Itr->message, recv_message.message, sizeof(char)*recv_message.size_of_blocks);
			//strcpy(temp,recv_message.message);
			//printf("temp: ");
			//print_msg(temp);
			//strcat(Itr->message,temp);
			//free(temp);
			//strcat(Itr->message,recv_message.message);
			printf("itr msg: ");
			print_msg(Itr->message);
			Itr->counter--;
			if(Itr->counter == 0){
				printf("About to send string: ");
				print_msg(Itr->message);
				close(sockfd);
				delete_node(Itr);
				return Itr;	
			}
			
		}
		
		
	}
	
	/*
		recv_message.hostIP = my_addr.sin_addr;		
		recv_message.port_number = my_addr.sin_port;	
		printf("Server Port : %d\n ",recv_message.port_number);	*/
		
}


/*
		we dont need number of messages now for the moment cos we will have an infinite loop till one buffer fills
		
		num_of_messages = atoi(recv_message.message);
		
		printf("atoi %d \n Receiving the rest of the packets\n",num_of_messages);
		for(x=1;x <=num_of_messages;x++)
		{ 
		  
		while(check!=0){
			
			if (recvfrom(sockfd, &recv_message, sizeof(recv_message), 0, (struct sockaddr*)&cli_addr, &slen)==-1)
			{	    	
				printf("Error: Recvfrom Failure\n");
				return;
			//printf("Block Sequence Number Recd %d\n", recv_message.seq_number);
		
			//printf("Received from client %s:%d:\t:%s\n\n",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port), recv_message.message);*/
