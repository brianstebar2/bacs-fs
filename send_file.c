/* SRiram
 * charmi */

#include "die_with_error.h"
#include "messages.h"
#include "send_file.h"

void send_file(char *path, char *IPaddr, uuid_t *uuids, uint64_t *num_uuids)
{
	FILE *fp = fopen(path,"r");
	char block[SIZE];
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
		already_read_block = fread(block, 1, SIZE, fp);
		create_msg_post_block_request(uuids[i], SIZE, block, &msg, &msg_len);
		parse_msg_get_block_response(msg, &uuid, &size, &content);
		if(get_header_resource(msg) != BACS_FILE || 
     	   	   get_header_action(msg) != POST ||
     	           get_header_type(msg) != BACS_RESPONSE)
    			die_with_error("send_file - invalid message header");
		i++;

		free(msg);
		free(content);
		i++;
	}
	fclose(fp);
}
