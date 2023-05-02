// Name       : Vishakha Kulkarni
// Student ID : 1632897

//Client Side Implementation

#include "headerfile.h"

int main(int argc, char**argv)
{
	int SOCK_Comm,n = 0, i = 1;
	char line_msg[30];
	FILE *FP;

	SOCK_Comm = socket(AF_INET,SOCK_DGRAM,0);

	struct req_pack req_pack;
	struct res_pack res_pack;
	//The SOCKADDR_IN structure specifies a transport address and port for the AF_INET address family
	struct sockaddr_in cl_Add;
	//Defines variable with integral type of length of at least 32 bits.
	socklen_t addrLen;

    // Socket Timeout - Struct timeval structure represents an elapsed time. 
    struct timeval timer;
    //To wait a whole number of seconds, you'd just set tv_sec.
	timer.tv_sec = 3;   
	//To wait a portion of a second,just set tv_usec.          
	timer.tv_usec = 0;


	// Checking Socket Connection
	//The setsockopt() function provides an application program with the means to control socket behavior
	setsockopt(SOCK_Comm, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timer,sizeof(struct timeval));
	int retry_count = 0;

	if(SOCK_Comm < 0) 
	{
		printf(RED "Socket Connection has Failed\n" RESET);
	}
    
    //bzero()function is used to set all the socket structures with null values
	bzero(&cl_Add,sizeof(cl_Add));
	//Filling server information
    //sin_family. The address family for the transport address. This member should always be set to AF_INET
	cl_Add.sin_family = AF_INET;
	//INADDR_ANY is a constant, that contain 0 in value . this will used only when you want connect from all active ports you don't care about ip-add 
	cl_Add.sin_addr.s_addr = htonl(INADDR_ANY);
	//The htons() function translates a short integer from host byte order to network byte order
	cl_Add.sin_port=htons(PORT);
	//Getting the size of the client address
	addrLen = sizeof cl_Add ;

	//Use Request Packet to load data into Packet by creating one
	req_pack = Initialize();

	//Getting contents of inputfile from text file
	FP = fopen("inputfile.txt", "rt");
    
    //Checking whether inputfile is present or not
	if(FP == NULL)
	{
		printf(RED "File Not Found\n");
	}
    
    //Checking inputfile's lines
	while(fgets(line_msg, sizeof(line_msg), FP) != NULL) 
	{
		retry_count = 0;
		n = 0;
		printf(RED " \n ------------- Start of a New Packet ------------ \n\n" RESET);
		//Extracting the contents of file.
		char * words;
		//Getting the Subscriber Number
		words = strtok(line_msg," ");
		req_pack.pack_len = strlen(words);
		req_pack.srcSubNo = (unsigned long) strtol(words, (char **)NULL, 10);
		//Getting the Subscriber's Technology and Packet Length
		words = strtok(NULL," ");
		req_pack.pack_len += strlen(words);
		req_pack.technology = atoi(words);
		//Getting the Segment Number
		words = strtok(NULL," ");
		req_pack.segment_No = i;

		//Printing contents of the Packet
		printpacket_info(req_pack);
		while(n <= 0 && retry_count < 3) 
		{ 
		//Sending Request to the Server to check if packet is sent, if not tries again.
		sendto(SOCK_Comm,&req_pack,sizeof(struct req_pack),0,(struct sockaddr *)&cl_Add,addrLen);
		
		//Receiving the response from the Server
		n = recvfrom(SOCK_Comm,&res_pack,sizeof(struct res_pack),0,NULL,NULL);
		if(n <= 0 )
		 	{		
				// If there is no response from the server
				printf(YEL "Out of Given Time\n\n" RESET);
				retry_count ++;
			}
		else if(n > 0) 
			{
			//When the response is recieved from the server
			printf(YEL "Status = " RESET);
			//Not paid for the subscription
			if(res_pack.type == NOT_PAID) {
				printf(YEL "Subscriber has not paid for the subscription.\n" RESET);
			}
			//Technology does not match
		    else if(res_pack.type == TECH_MISMATCH)
		    {
				printf(YEL "Subscriber is found but it's technology doesn't match.\n" RESET);
			}
			//Not exist in the verification database
		    else if(res_pack.type == NOT_EXIST ) 
			{
				printf(YEL "Subscriber does not exist.\n" RESET);
			}
			//Paid for subscription and present in the verification database then access is given
		    else if(res_pack.type == PAID) 
		    {
				printf(YEL "Subscriber has given access to the network.\n" RESET);
			}
	    }
	}
		// Make upto 3 attempts for server to respond
		if(retry_count >= 3 ) 
		{
			printf(RED "Server is not responding.\n\n" RESET);
			exit(0);
		}
		i++;
		
	}
	fclose(FP);
};