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

struct sockaddr_in my_addr;
int sockfd_rcv;

void socket_receive_create(int PN)
{
	if ((sockfd_rcv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		printf("Error: Socket Not Created\n");
	else
		printf("Socket_receive created at port %d\n",PN);
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = PN;
	if (bind(sockfd_rcv, (struct sockaddr* ) &my_addr, sizeof(my_addr))==-1)		
		printf("Error: Bind Failure\n");
}

void socket_receive_close()
{
	close(sockfd_rcv);
}

struct Node* myrecv(int PN)
{	
	struct Send_message recv_message;
	socklen_t slen=sizeof(my_addr);
	char ackbuf[BUFLEN];
	struct Node* Itr;
	Itr = head = NULL;
	/*printf("Inside myrecv\n");*/
	
	while(1){
		/*printf("\n");*/
		memset((void*)&recv_message,0,sizeof(recv_message));
		if (recvfrom(sockfd_rcv, &recv_message, sizeof(recv_message), 0, (struct sockaddr*)&my_addr, &slen)==-1)
		{	    	
			printf("Error: Recvfrom Failure\n");
			return 0;
		}
	/*	printf("Message received: ");
		print_msg(recv_message.message);*/
		memset(ackbuf, 0, sizeof(ackbuf));
		strcpy(ackbuf,"ACK");	
		if (sendto(sockfd_rcv, ackbuf, BUFLEN, 0, (struct sockaddr*)&my_addr, slen)==-1)
		{
			printf("Error: Sendto Failure\n");
			return 0;
		}
		/*printf("ACK SENT\n");*/
		if(recv_message.seq_number==0){
		/*printf("seq number = 0\n");*/
			if(head == NULL){
				int total_blocks = ceil((float)recv_message.size_of_blocks/BUFLEN);
				/*printf("head is null\n");*/
				create_new_node(head,recv_message.seq_number, my_addr.sin_addr.s_addr, recv_message.port_number,recv_message.size_of_blocks,total_blocks);
			}
			else{
			int total_blocks = ceil((float)recv_message.size_of_blocks/BUFLEN);
			/*printf("head not null\n");*/
			while(Itr->next!=NULL){  		
				Itr = Itr->next;
			}
			    
			    create_new_node(Itr,recv_message.seq_number, my_addr.sin_addr.s_addr, recv_message.port_number,recv_message.size_of_blocks,total_blocks);
			}
		}	
		else{
			/*printf("\n\n");*/
			Itr = head;
			while((Itr->IP != my_addr.sin_addr.s_addr)&&(Itr->next!=NULL)){
				Itr = Itr->next;
			}
			memcpy(Itr->message, recv_message.message, sizeof(char)*recv_message.size_of_blocks);
			Itr->counter--;
			if(Itr->counter == 0){
				/*printf("About to send string: ");
				print_msg(Itr->message);*/
				delete_node(Itr);
				return Itr;	
			}	
		}	
	}		
}
