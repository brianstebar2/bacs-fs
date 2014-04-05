#include "stdio.h"
#include "UDPclient.h"

int main(int argv, char** argc)
{	
	char IPaddr[1024];
	char ch;
	char data[1024];
	int data_testint;
	int choice;
	strcpy(IPaddr, "127.0.0.1");
	ErrorCode mycode;
	loop: printf("Enter your choice\n1: Send Test String\n2: Send Heart beat\n\nChoice: ");//2: Transmit Int\n");
	scanf("%d",&choice);
	switch(choice)
	{
		case 1:
		{
			strcpy(data, strcat( IPaddr, " My Test String "));
			printf("data : %s\n",data);
			mycode = mysend(data,  IPaddr);
			break;
		}
		case 2: 
		{
			//data_testint = htonl(123);
			strcpy(data, strcat( IPaddr, " Heartbeat "));
			mycode = mysend(data,  IPaddr);
			break;
		}
		/*case 3:
		{	
			int num = 1000;
			printf("\n\nSending number %d\n", num);
			mycode = myclient(&num,  IPaddr);
			break;
		}*/
		default:
		{
			printf("Enter 1, 2 or 3\n");
			goto loop;		
		}
	}
	//mycode = myclient(data,  IPaddr);
	if(mycode == RETRY)
	{
		printf("Sender Returned: %d\n", mycode);
		printf("Need to resend data\nResending...\n\n");
		printf("Sender Returned: %d\n",mysend(data, IPaddr));		
	}
	if(mycode == FAILURE)
	{
		printf("Sender Returned: %d\tSending Failed\n", mycode);
	
	}  
	if(mycode == SUCCESS)
	{
		printf("Sender Returned: %d\tSending SUccessful\n", mycode);
	}
	
	//printFunction();
	return 0;
}
