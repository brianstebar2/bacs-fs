#include "UDPserver.h"
int main()
{	void* ans;
	while((ans = myrecv())!=1 ||(ans = myrecv())!=2)
	{
		fputs(ans, stdout);
		printf("\n");
	}
	return 0;
}
