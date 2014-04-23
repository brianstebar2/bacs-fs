/*UDPClient.c*/


#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "UDPclient.h"
#include "math.h"



ErrorCode mysend(void* p, char* IPaddr, int PN, int size_of_blocks  )
{

	struct Send_message send_message;

	int total_blocks = ceil(size_of_blocks/BUFLEN);

	struct sockaddr_in serv_addr;
	int sockfd, i, slen=sizeof(serv_addr);
	
	char rcvbuf[BUFLEN];
	
	char* rp = p;
	char tot[3];
	

	/*Timer variables*/
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	{
	
		printf("Error: Socket Not Created\n");
		return FAILURE;
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);


	if (inet_aton(IPaddr, &serv_addr.sin_addr)==0)
	{
		return FAILURE;
	}

	
	
	
	/*The first packet contains the number of blocks as the message */
			
	

	for(i = 0; i <= total_blocks; i++)
	{
		memset(rcvbuf, 0, BUFLEN);
		send_message.seq_number = i;
		
		if(i==0)
		{
			sprintf(send_message.message, "%d", total_blocks);
			printf("Number of packets to be sent %s\n", send_message.message);
		}
		else
		{
			memcpy(send_message.message, rp, MESSAGE_LEN);
			rp += MESSAGE_LEN;
		}

		if (sendto(sockfd,	&send_message, sizeof(send_message), 0, (struct sockaddr*)&serv_addr, slen)==-1)  /*replacing buf with voidbuf*/
		{
			printf("sendto: Error\n\n");
			return FAILURE;
		}
		else
			printf("sent: %s\n", send_message.message);
		
		/*Configure a recieve timer*/
		if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,sizeof(timeout)) < 0)
		{
			printf("setsockopt: Error\n\n");
		}
		/*Receive ACK from Server*/
		if (recvfrom(sockfd, rcvbuf, BUFLEN, 0, (struct sockaddr*)&serv_addr, &slen)==-1)
		{    
			return RETRY;
		}
		else
		{
			printf("Received response from server %s:%d:\t:%s\n\n",inet_ntoa(serv_addr.sin_addr),ntohs(serv_addr.sin_port), rcvbuf);
			/*return SUCCESS;*/
		}
	}
	
	
	

	close(sockfd);
	/*free (rp);*/
}
