#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h>



#define BUFLEN 1500
#define MESSAGE_LEN 1496
#define PORT 9930

struct Send_message
{
	struct in_addr hostIP;
	int port_number;
	int seq_number;
	char message[1496];
};

typedef enum {SUCCESS, FAILURE, RETRY} ErrorCode;



ErrorCode mysend(void* p, char* IPaddr, int PN, int size_of_blocks);

