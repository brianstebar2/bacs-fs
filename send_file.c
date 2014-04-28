/* SRiram
 * charmi */

#include "die_with_error.h"
#include "messages.h"
#include "send_file.h"

int send_file(char *path, long IPaddr, uuid_t *uuids, uint64_t num_uuids, int PN)
{
	FILE *fp = fopen(path,"r");
	char block[DEFAULT_BLOCK_SIZE];
	int i=0;
	fseek(fp,0,SEEK_SET);
	while(!feof(fp))
	{
		char *msg = 0, *err_msg_string = 0;
		uint64_t msg_len;
		int num;
		uuid_t uuid;
		char *resp_msg = 0;
		ErrorCode error;
		struct Node* resp;
		if(i>=num_uuids)
		{
			printf("****************error num_uuids");
			fclose(fp);
			return 0;
		}
		else
		{
			memset(block,0,sizeof(block));
			num = fread(block, 1, DEFAULT_BLOCK_SIZE, fp);
			create_msg_post_block_request(uuids[i], num, block, &msg, &msg_len);
			error = mysend(msg, IPaddr, PN, msg_len);
			if(error == FAILURE || error == RETRY)
			{
				printf("error in send");
				free(msg);
				fclose(fp);
				return 0;
			}
			else
			{
				resp = myrecv(CLIENT_PORT);
				resp_msg = resp->message;
				if(get_header_type(resp_msg) == BACS_ERROR)
				{
      					parse_msg_error(resp_msg, &err_msg_string);
					printf("%s\n",err_msg_string);
					fclose(fp);
					return 0;
				}
    				else
					parse_msg_post_block_response(resp_msg, &uuid);
			}
			free(err_msg_string);
			free(resp->message);
			free(resp);
			free(msg);
		}
		i++;
	}
	printf("\n...File uploaded successfully\n");
	fclose(fp);
	return 1;
}
