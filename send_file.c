/* SRiram
 * charmi */

#include "die_with_error.h"
#include "messages.h"
#include "send_file.h"

void send_file(char *path, char *IPaddr, uuid_t *uuids, uint64_t num_uuids)
{
	FILE *fp = fopen(path,"r");
	char block[DEFAULT_BLOCK_SIZE];
	size_t already_read_block;
	int i=0;
	fseek(fp,0,SEEK_SET);

	while(!feof(fp))
	{
		char *msg = 0;
		uint64_t msg_len;
		uuid_t uuid;
		uint32_t size;
		char *content = 0;
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
		already_read_block = fread(block, 1, DEFAULT_BLOCK_SIZE, fp);
		create_msg_post_block_request(uuids[i], DEFAULT_BLOCK_SIZE, block, &msg, &msg_len);
		/* mysend(msg...) */
		/* Wait for server response */
		/* myrecv(..., resp_msg); */
		parse_msg_post_block_response(resp_msg, &uuid, &size, &content);
		if(get_header_resource(resp_msg) != BACS_FILE || 
     	   	   get_header_action(resp_msg) != POST ||
     	           get_header_type(resp_msg) != BACS_RESPONSE)
    			die_with_error("send_file - invalid message header");
		i++;
		free(resp_msg);
		free(msg);
		free(content);
	}
	fclose(fp);
}
