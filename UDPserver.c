#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>
#include "UDPserver.h"

/*void err(char *str)
{
    perror(str);
    exit(1);
}*/

struct Send_message myrecv(void)
{
	struct sockaddr_in my_addr, cli_addr;
	int sockfd, i, x; 
	socklen_t slen=sizeof(cli_addr);
	struct Send_message recv_message;
	char ackbuf[BUFLEN];
	char* final_string = "\0" ;
	int num_of_messages;
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	{
		printf("Error: Socket Not Created\n");
		return; /* (void*)FAILURE; */
	}	
	
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr* ) &my_addr, sizeof(my_addr))==-1)		
	{	
		printf("Error: Bind Failure\n");
		return; /* (void*)FAILURE; */
	}	
	/*memset(voidbuf, 0, sizeof(voidbuf));*/
	if (recvfrom(sockfd, &recv_message, sizeof(recv_message), 0, (struct sockaddr*)&cli_addr, &slen)==-1)
	{	    	
		printf("Error: Recvfrom Failure\n");
		return;/* (void*)FAILURE; */
	}
	printf("Number of Blocks to be received %d\n", recv_message.seq_number);
	
	/*printf("Received from client %s:%d:\t:%s\n\n",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port), voidbuf);*/
	memset(ackbuf, 0, sizeof(ackbuf));
	strcpy(ackbuf,"ACK");		
	if (sendto(sockfd, ackbuf, BUFLEN, 0, (struct sockaddr*)&cli_addr, slen)==-1)
	{
		printf("Error: Sendto Failure\n");
		return;/* (void*)FAILURE; */
	}
	
	num_of_messages = atoi(recv_message.message);
	printf("atoi %d \n Receiving the rest of the packets\n",num_of_messages);
	for(x=1;x <=num_of_messages;x++)
	{
		
		printf("Entered the for loop\n");
		/*This loop runs till all the packets to a particular message are received*/
		if (recvfrom(sockfd, &recv_message, sizeof(recv_message), 0, (struct sockaddr*)&cli_addr, &slen)==-1)
		{	    	
			printf("Error: Recvfrom Failure\n");
			return;/* (void*)FAILURE; */
		}
		printf("Block Sequence Number Recd %d\n", recv_message.seq_number);
		
		printf("Received from client %s:%d:\t:%s\n\n",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port), recv_message.message);
		/*strcat(final_string, recv_message.message);*/
		memset(ackbuf, 0, sizeof(ackbuf));
		strcpy(ackbuf,"ACK");		
		if (sendto(sockfd, ackbuf, BUFLEN, 0, (struct sockaddr*)&cli_addr, slen)==-1)
		{
			printf("Error: Sendto Failure\n");
			return;/* (void*)FAILURE; */
		}
	}
	
	
	
	recv_message.hostIP = my_addr.sin_addr;		/*Populate the struct with the IP address*/
	recv_message.port_number = my_addr.sin_port;	/*Populate the struct with the port*/
	printf("Server Port : %d\n ",recv_message.port_number);	
	close(sockfd);
	return recv_message;		/*returns the last packet received. Must be changed to return the entire string*/
}

