#include "stdio.h"
#include "UDPclient.h"
#include <math.h>

int main(int argv, char** argc)
{	
	char IPaddr[1024];
	char ch;
	//char data[1024];
	int data_testint;
	int choice;
	strcpy(IPaddr, "127.0.0.1");
	ErrorCode mycode;

	char* data = "This is a test.> I have a doubt in Q1 a) of the Midterm where it is stated that  Do not modify the way in which shares are stored or the information that is stored with them . If the same share is written to different servers and yet each server only holds a single share, would this be in violation of how the shares are stored? There are two attacks possible when data is stored using secretsharing techniques.  One attack is by a faulty client that generatesinconsistent shares during writes, i.e., different subsets of k sharesout of the q shares will decode to different values.  The other attack is when a faulty server returns incorrect or arbitrary sharesduring reads.  Such attacks can be detected using veriﬁable secretsharing schemes [6]. In such schemes, some common data (callewitnesses) for all the shares is computed by a client during writes and sent to all the servers.  Before storing the shares and the wit-nesses,the servers check the shares received against the witnessesand arrive at a consensus on the consistency of the shares. Duringreads, a client will ﬁrst determine the witnesses and check the va-lidity of each share with the witnesses before proceeding to decode the sensitive data.  Veriﬁable secret sharing schemes signiﬁcantlyincrease the computation overheads during the secret sharing (en-coding) and secret recovery (decoding) processes.  A widely useethod for veriﬁable secret sharing is Feldman’s scheme [8].  Ta-ble 2 gives the computation times during secret sharing and secretrecovery of an 8 KB block of data when Feldman’s scheme is usedwith Shamir’s scheme.For comparison purposes, the throughputs of the AES Rijndaelsymmetric-key encryption algorithm are given in Table 3.From Tables 1–3, it is clear that the computation times of Samir’sscheme and Feldman’s scheme are far higher than those of symmetric-key encryption and, in fact, this performance is well below whatis acceptable for modern data storage systems.  The secret recov-ery computation time for veriﬁable secret sharing are at least 3000times slower than the Rijndael decryption times.  The above anal-ses also indicate, in part, why perfect secret sharing techniqueshave not been adopted for generic data to date. We reduce the computation overheads by using the following two mechanisms:Mechanism 1: Use a (q, q) perfect secret sharing scheme: Whenk = q, i.e., all the shares are needed to recover the secret, then“inconsistent” secret sharing is not possible.  That is, there is noquestion of different subsets of k shares out of q shares decoding toifferent values because there is only one such subset, since k = q.Hence, veriﬁable secret sharing schemes can be avoided.  Further,a (q, q) perfect secret sharing scheme can be realized using sim-ple bit-wise XOR operations.  If each data bit is thought of as aseparate secret, then each share is a single bit and XOR of the qshares (or q bits) gives the encoded secret bit.  In practice, XORsecret sharing can be implemented with word-wide operations forefﬁciency.  Table 4 lists the computation times during secret shar-ing and secret recovery for a selection of (q, q) values for XORsecret sharingNote that XOR secret sharing is also a perfect seThis is a test.> I have a doubt in Q1 a) of the Midterm where it is stated that  Do not modify the way in which shares are stored or the information that is stored with them . If the same share is written to different servers and yet each server only holds a single share, would this be in violation of how the shares are stored? There are two attacks possible when data is stored using secretsharing techniques.  One attack is by a faulty client that generatesinconsistent shares during writes, i.e., different subsets of k sharesout of the q shares will decode to different values.  The other attack is when a faulty server returns incorrect or arbitrary sharesduring reads.  Such attacks can be detected using veriﬁable secretsharing schemes [6]. In such schemes, some common data (callewitnesses) for all the shares is computed by a client during writes and sent to all the servers.  Before storing the shares and the wit-nesses,the servers check the shares received against the witnessesand arrive at a consensus on the consistency of the shares. Duringreads, a client will ﬁrst determine the witnesses and check the va-lidity of each share with the witnesses before proceeding to decode the sensitive data.  Veriﬁable secret sharing schemes signiﬁcantlyincrease the computation overheads during the secret sharing (en-coding) and secret recovery (decoding) processes.  A widely useethod for veriﬁable secret sharing is Feldman’s scheme [8].  Ta-ble 2 gives the computation times during secret sharing and secretrecovery of an 8 KB block of data when Feldman’s scheme is usedwith Shamir’s scheme.For comparison purposes, the throughputs of the AES Rijndaelsymmetric-key encryption algorithm are given in Table 3.From Tables 1–3, it is clear that the computation times of Samir’sscheme and Feldman’s scheme are far higher than those of symmetric-key encryption and, in fact, this performance is well below whatis acceptable for modern data storage systems.  The secret recov-ery computation time for veriﬁable secret sharing are at least 3000times slower than the Rijndael decryption times.  The above anal-ses also indicate, in part, why perfect secret sharing techniqueshave not been adopted for generic data to date. We reduce the computation overheads by using the following two mechanisms:Mechanism 1: Use a (q, q) perfect secret sharing scheme: Whenk = q, i.e., all the shares are needed to recover the secret, then“inconsistent” secret sharing is not possible.  That is, there is noquestion of different subsets of k shares out of q shares decoding toifferent values because there is only one such subset, since k = q.Hence, veriﬁable secret sharing schemes can be avoided.  Further,a (q, q) perfect secret sharing scheme can be realized using sim-ple bit-wise XOR operations.  If each data bit is thought of as aseparate secret, then each share is a single bit and XOR of the qshares (or q bits) gives the encoded secret bit.  In practice, XORsecret sharing can be implemented with word-wide operations forefﬁciency.  Table 4 lists the computation times during secret shar-ing and secret recovery for a selection of (q, q) values for XORsecret sharingNote that XOR secret sharing is also a perfect se";


	printf("sizeof data: %lu\n", strlen(data));
	mycode = mysend(data,  IPaddr , 9930 , strlen(data));
	//printf("Enter your choice\n1: Send Test String\n2: Send Heart beat\n\nChoice: ");//2: Transmit Int\n");
	
	/*loop: printf("Enter your choice\n1: Send Test String\n2: Send Heart beat\n\nChoice: ");//2: Transmit Int\n");
	scanf("%d",&choice);
	switch(choice)
	{
		case 1:
		{
			//strcpy(data, strcat( IPaddr, " My Test String "));//Append Sender IP to the message
			strcpy(data, "My test string");
			printf("data : %s\n",data);
			mycode = mysend(data,  IPaddr);
			break;
		}
		case 2: 
		{
			//data_testint = htonl(123);
			//strcpy(data, strcat( IPaddr, " Heartbeat "));
			strcpy(data,"Hearbeat");
			mycode = mysend(data,  IPaddr);
			break;
		}
		
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
	*/
	//printFunction();
	//free(data);
	return 0;
}
