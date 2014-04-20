/* SRiram
 * charmi */

#include "send_file.h"

void send_file(char *path, char *IPaddr)
{
	FILE *fp = fopen(path,"r");
	char block[SIZE];
	size_t already_read_block;
	int mycode;
	char data[SIZE];

	fseek(fp,0,SEEK_SET);

	while(!feof(fp))
	{
		/* Set block initially to 0 */
		memset(block,0,sizeof(block));
		/* Caller should check for -1 --> Empty file! */
		if(fp==NULL)
			{
			printf("\n ERROR - EMPTY FILE!");
				/* return EOF; */
			}
		already_read_block = fread(block, 1, SIZE, fp);
		memset(data, 0, SIZE);
		/* sending blocks */
		strcpy(data, strcat( IPaddr, block));
		/* printf("data : %s\n",data); */
		mycode = mysend(data,  IPaddr);
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
	
	}
	fclose(fp);
}
