// Name       : Vishakha Kulkarni
// Student ID : 1632897

//Server Side Implementation

#include "headerfile.h"

int main(int argc, char**argv){
	
	int SOCK_Comm,n;

    struct req_pack req_pack;
	struct res_pack res_pack;
	struct SubDB SubDB[LENGTH];  
	//The SOCKADDR_IN structure specifies a transport address and port for the AF_INET address family  
	struct sockaddr_in ser_Add;
	struct sockaddr_storage serverStorage;
	//Defines variable with integral type of length of at least 32 bits.
	socklen_t addrLen;
	SOCK_Comm=socket(AF_INET,SOCK_DGRAM,0);
    
    //Reading Verification Database file from the Subscriber Database and Extract file Contents
	read_SubDB(SubDB);
	//bzero()function is used to set all the socket structures with null values
    bzero(&ser_Add,sizeof(ser_Add));
    //Filling client information
    //sin_family. The address family for the transport address. This member should always be set to AF_INET
	ser_Add.sin_family = AF_INET;
	//INADDR_ANY is a constant, that contain 0 in value . this will used only when you want connect from all active ports you don't care about ip-add 
	ser_Add.sin_addr.s_addr=htonl(INADDR_ANY);
	//The htons() function translates a short integer from host byte order to network byte order
	ser_Add.sin_port=htons(PORT);
	//Getting the size of the server address
	addrLen = sizeof ser_Add;

	//Bind the socket with the server address
    //The bind() function binds a unique local name to the socket with descriptor socket
    bind(SOCK_Comm,(struct sockaddr *)&ser_Add,sizeof(ser_Add));
	printf(RED "\n\nServer has been deployed and is running successfully\n\n\n" RESET);
	
    for (;;) 
    {
    //Receieve the packet from the client
	n = recvfrom(SOCK_Comm,&req_pack,sizeof(struct req_pack),0,(struct sockaddr *)&serverStorage, &addrLen);
	//Printing the received packet from the client
	printpacket_info(req_pack);
	if(req_pack.segment_No == 11) 
		{
			exit(0);
		}
    // Checking access permission
	if(n > 0 && req_pack.Acc_Per == 0XFFF8)
		{
		//Creating the response packet
		res_pack = createRES_pack(req_pack);
		//Getting the Subscriber Identity
		int result = check(SubDB,req_pack.srcSubNo,req_pack.technology);
		    if(result == 0) 
		       {
				res_pack.type = NOT_PAID;
				printf(YEL "Subscriber has not paid for the subscription.\n" RESET);
			    }
			else if(result == 1) 
			    {
				printf(YEL "Subscriber has paid for the subscription.\n" RESET);
				res_pack.type = PAID;
			    }
			else if(result == -1) 
			    {
				printf(YEL "Subscriber does not exist.\n" RESET);
				res_pack.type = NOT_EXIST;
			    }          
            else
               {              
                printf(YEL "Subscriber's technology does not match.\n" RESET);
            	res_pack.type = TECH_MISMATCH;
                }                        
			//Sending response packet to the client
		sendto(SOCK_Comm,&res_pack,sizeof(struct res_pack),0,(struct sockaddr *)&serverStorage,addrLen);
		}
		n = 0;
		printf(RED" \n --------------- Start of a New Packet ------------- \n" RESET);
	}
}