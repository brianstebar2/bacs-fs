#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h>
#define BUFLEN 1024
#define PORT 9930

#include <stdbool.h>
#include "UDPserver.h"

void err(char *str)
{
    perror(str);
    exit(1);
}



void* myrecv(void)
{
	struct sockaddr_in my_addr, cli_addr;
	int sockfd, i; 
	socklen_t slen=sizeof(cli_addr);
	char buf[BUFLEN];
	
	void* voidbuf;
	voidbuf = malloc (sizeof(buf));
	
	char ackbuf[BUFLEN];
	bool flag = false;
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	{
      		//err("socket");
		printf("Error: Socket Not Created\n");
		return (void*)FAILURE;
	}	
	//else
	{ 
	//	printf("Server : Socket() successful\n");
	}
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr* ) &my_addr, sizeof(my_addr))==-1)
	{	
		//err("bind");
		printf("Error: Bind Failure\n");
		return (void*)FAILURE;
	}	
	
	//while(1)
	{
		memset(voidbuf, 0, sizeof(voidbuf));
		if (recvfrom(sockfd, voidbuf, BUFLEN, 0, (struct sockaddr*)&cli_addr, &slen)==-1)
		{	    	
			//err("recvfrom()");
			printf("Error: Recvfrom Failure\n");
			return (void*)FAILURE;
		}
		//--------DEBUG---------------
		//printf("Received from client %s:%d:\t:%s\n\n",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port), voidbuf);
		memset(ackbuf, 0, sizeof(ackbuf));
		strcpy(ackbuf,"ACK");		
		if (sendto(sockfd, ackbuf, BUFLEN, 0, (struct sockaddr*)&cli_addr, slen)==-1)
		{
			printf("Error: Sendto Failure\n");
			return (void*)FAILURE;
		}	
		
	}

	close(sockfd);
	return voidbuf;
}

