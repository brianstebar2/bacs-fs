//UDPClient.c


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

#define BUFLEN 1024
#define PORT 9930


void err(char *s)
{
	perror(s);
	exit(1);
}

ErrorCode mysend(void* p, char* IPaddr )
{	

	
	struct sockaddr_in serv_addr;
	int sockfd, i, slen=sizeof(serv_addr);
	char buf[BUFLEN];
	char rcvbuf[BUFLEN];
	void* voidbuf;
	voidbuf = malloc(BUFLEN);
	
	//Timer variables
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0; 

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	{
		//err("socket");
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

	//while(1)
	{
		//printf("Clear the rcvbuf buffer\n");
		memset(rcvbuf, 0, BUFLEN);
		memcpy(buf, p, BUFLEN);			
		if(strcmp(buf,"exit") == 0)
		{
			return FAILURE;
		}

		printf("Sending Data\n");
		if (sendto(sockfd,buf, BUFLEN, 0, (struct sockaddr*)&serv_addr, slen)==-1)		//replacing buf with voidbuf
		{
			//err("sendto()");
			printf("sendto: Error\n\n");
			return FAILURE;
		}

		//Configure a recieve timer
		if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,sizeof(timeout)) < 0)
		{
			printf("setsockopt: Error\n\n");
			
		}
		//Receive ACK from Server
		if (recvfrom(sockfd, rcvbuf, BUFLEN, 0, (struct sockaddr*)&serv_addr, &slen)==-1)
		{	    	
			return RETRY;
		}
		else
		{
			printf("Received response from server %s:%d:\t:%s\n\n",inet_ntoa(serv_addr.sin_addr),ntohs(serv_addr.sin_port), rcvbuf);
			return SUCCESS;
		}
	}

	close(sockfd);
	free(voidbuf);
	
}
