/* SRiram
 * charmi */

#include "die_with_error.h"
#include "messages.h"
#include "send_file.h"

void send_file(char *path, long IPaddr, uuid_t *uuids, uint64_t num_uuids, int PN)
{
	FILE *fp = fopen(path,"r");
	char block[DEFAULT_BLOCK_SIZE];
	int i=0;
	fseek(fp,0,SEEK_SET);
	while(!feof(fp))
	{
		char *msg = 0, *err_msg_string = 0;
		uint64_t msg_len;
		uuid_t uuid;
		/*uint32_t size;*/
		char *content = 0;
		char *resp_msg = 0;
		ErrorCode error;
		struct Node* resp;
		if(i>=num_uuids)
			printf("****************error num_uuids");
		/* Set block initially to 0 */
		memset(block,0,sizeof(block));
		/* Caller should check for -1 --> Empty file! */
		if(fp==NULL)
			{
			printf("\n ERROR - EMPTY FILE!");
				/* return EOF; */
			}
		fread(block, 1, DEFAULT_BLOCK_SIZE, fp);
		create_msg_post_block_request(uuids[i], DEFAULT_BLOCK_SIZE, block, &msg, &msg_len);
		/* mysend(msg...) */
		error = mysend(block, IPaddr, PN, DEFAULT_BLOCK_SIZE);
		if(error == FAILURE || error == RETRY)
			printf("error in send");
		
		/* Wait for server response */
		/* myrecv(..., resp_msg); */
		resp = myrecv(CLIENT_PORT);
		resp_msg = resp->message;
		if(get_header_type(resp_msg) == BACS_ERROR)
		{
      			parse_msg_error(resp_msg, &err_msg_string);
			printf("%s\n",err_msg_string);
		}
    		else
			parse_msg_post_block_response(resp_msg, &uuid);
		
		i++;
		free(err_msg_string);
		free(resp->message);
		free(resp);
		free(msg);
		free(content);
	}
	fclose(fp);
}
