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
#include "messages.h"

struct sockaddr_in serv_addr;
int sockfd;

void socket_send_create(int PN)
{
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		printf("Error: Socket Not Created\n");
	else
		printf("Socket_send created at port %d\n",PN);
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = PN;	
}

void socket_send_close()
{
	close(sockfd);
}

ErrorCode mysend(void* p, long IPaddr, int PN, int size_of_blocks)
{
	struct Send_message send_message;
	int total_blocks = ceil((float)size_of_blocks/BUFLEN);
	int  i;
	socklen_t slen=sizeof(serv_addr);
	char rcvbuf[BUFLEN];
	struct timeval timeout;
	char* rp = p;
	/*print_msg(rp);*/
	send_message.size_of_blocks = size_of_blocks;
	/*Timer variables*/
	timeout.tv_sec = 15;
	timeout.tv_usec = 0;
	for(i = 0; i <= total_blocks; i++)
	{
		int j;
		memset(rcvbuf, 0, BUFLEN);
		send_message.seq_number = i;
		if(i==0)
		{
			/*sprintf(send_message.message, "%d", total_blocks);
			printf("Number of packets to be sent %s\n", send_message.message);*/
		}
		else
		{
			memcpy(send_message.message, rp, MESSAGE_LEN);
			rp += MESSAGE_LEN;
		}

		
		
		for(j=0; j<2; j++)
		{
			if (sendto(sockfd, &send_message, sizeof(send_message), 0, (struct sockaddr*)&serv_addr, slen)==-1)
			{
				printf("sendto: Error\n\n");
				return FAILURE;
			}
			else
			{
				/*printf("sent: ");
				print_msg(send_message.message);*/
			}
			/*Configure a recieve timer*/
			if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,sizeof(timeout)) < 0)
				printf("setsockopt: Error\n\n");
			/*Receive ACK from Server*/
			if (recvfrom(sockfd, rcvbuf, BUFLEN, 0, (struct sockaddr*)&serv_addr, &slen)==-1)
			{
				printf("No response received\n");
				if(j==0)
					printf("Resending packet....\n");
			}
			else
			{
				/*printf("Received response from server %s:%d:\t:%s\n\n",inet_ntoa(serv_addr.sin_addr),ntohs(serv_addr.sin_port), rcvbuf);*/
				break;
			}
		}
		if(j==2)
			return FAILURE;
	}	
	return SUCCESS;
}
