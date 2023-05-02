// Name       : Vishakha Kulkarni
// Student ID : 1632897

//Required Libraries
#include <sys/socket.h> //Defines macros like CMSG_DATA, CMSG_NXTHDR to gain access to the data arrays
#include <netinet/in.h> //Defines IN6ADDR_LOOPBACK_INIT macro and it is defined by systsem
#include <stdio.h>      //Standard I/O O/P
#include <strings.h>    //Contains macro definitions, constants, declarations of functions
#include <stdint.h>     //Declare sets of integer types having specified widths, and shall define corresponding sets of macros.
#include <stdlib.h>     //For memory allocation, process control, conversions etc.
#include <unistd.h>     //Defines miscellaneous symbolic constants and types, and declares miscellaneous functions.
#include <string.h>     //Contains macro definitions, constants, declarations of functions
#include <time.h>       //Defines the timeval structure 


#define PORT 1626                 // Port Number - UDP used for communicaion
#define LENGTH 10                 // Length of a packet

// Reject Codes
#define PAID                     0XFFFB    //Paid for Subscription,can access network
#define NOT_PAID                 0XFFF9    //Not Paid for Subscription,can not access network
#define NOT_EXIST                0XFFFA    //Subscriber does not exist in the Verification Database
#define TECH_MISMATCH            0XFFFC    //Subscriber's technology does not match with the Verification Database's subscriber's technology

//Defining Colors
#define RED    "\x1B[31m"
#define YEL    "\x1B[33m"
#define RESET  "\x1B[0m"

//Response Packet Format
struct res_pack 
{
	uint16_t packetID;      //Start of Packet Identifier - 0XFFFF
	uint8_t clientID;       // Client ID (255 Decimal) - 0XFF
	uint16_t type;
	uint8_t segment_No;      //Sequence Number
	uint8_t pack_len;        // Packet Length:- max Length (255 Decimal) - 0XFF
	uint8_t technology;       // Technology of Subscriber - 2G,3G,4G,5G
	unsigned long srcSubNo;   //Source Subscriber Number - 0XFFFFFFFF (4294967295 Decimal)
	uint16_t endpacketID;       // End of Packet identifier - 0XFFFF
};

//Request Packet Format
struct req_pack
{
	uint16_t packetID;         	//Start of Packet Identifier - 0XFFFF
	uint8_t clientID;          	// Client ID (255 Decimal) - 0XFF
	uint16_t Acc_Per;         	 // Acess permission - 0XFFF8
	uint8_t segment_No;              //Sequence Number
	uint8_t pack_len;        	  // Packet Length:- max Length (255 Decimal) - 0XFF
	uint8_t technology;       	 // Technology of Subscriber - 2G,3G,4G,5G
	unsigned long long srcSubNo;      //Source Subscriber Number - 0XFFFFFFFF (4294967295 Decimal)
	uint16_t endpacketID;		 // End of Packet identifier - 0XFFFF
};

//Request Packet Format: Generating a Request Packet and assign to default values
struct req_pack Initialize () 
{
	struct req_pack req_pack;
	req_pack.packetID = 0XFFFF;
	req_pack.clientID = 0XFF;
	req_pack.Acc_Per = 0XFFF8;
	req_pack.endpacketID = 0XFFFF;
	return req_pack;
}

// Subscriber Verification Database Format
struct SubDB 
{
	unsigned long SubNo;
	uint8_t technology;
	int paidStatus;   //Status subscriber: 1 for paid and 0 for not paid
};

//Printing Packet Information
void printpacket_info(struct req_pack req_pack ) 
{
	printf("Packet ID :  %x\n",req_pack.packetID);
	printf("Client ID : %hhx\n",req_pack.clientID);
	printf("Access Permission : %x\n",req_pack.Acc_Per);
	printf("Segment no : %d \n",req_pack.segment_No);
	printf("Length of the Packet : %d\n",req_pack.pack_len);
	printf("Technology : %d \n", req_pack.technology);
	printf("Subscriber no : %llu \n",req_pack.srcSubNo);
	printf("End of RequestPacket ID : %x \n\n\n",req_pack.endpacketID);
}


//Response Packet Format
struct res_pack createRES_pack(struct req_pack req_pack) 
{
	struct res_pack res_pack;
	res_pack.packetID = req_pack.packetID;
	res_pack.clientID = req_pack.clientID;
	res_pack.segment_No = req_pack.segment_No;
	res_pack.pack_len = req_pack.pack_len;
	res_pack.technology = req_pack.technology;
	res_pack.srcSubNo = req_pack.srcSubNo;
	res_pack.endpacketID = req_pack.endpacketID;
	return res_pack;
}

//Reading Verification Database file from the Subscriber Database and Extract file Contents
void read_SubDB(struct SubDB SubDB[])
{
	char line_msg[30];
	int n = 0;
	FILE *FP;
        
        //Getting contents of Verification_Database from text file
	FP = fopen("Verification_Database.txt", "rt");
	//Checking whether inputfile is present or not
	if(FP == NULL)
	{
		printf(RED "File not found\n");
		return;
	}
	//If Verification_Database file is not empty then
	while(fgets(line_msg, sizeof(line_msg), FP) != NULL)
	{
		char * words=NULL;
		//Getting the Subscriber Number
		words = strtok(line_msg," ");
		SubDB[n].SubNo =(unsigned) atol(words);
		//Getting the Subscriber's Technology
		words = strtok(NULL," ");
		SubDB[n].technology = atoi(words);	
		//Getting the Subscriber's Paid Status
		words = strtok(NULL," ");
		SubDB[n].paidStatus = atoi(words);
		n++;
	}
	fclose(FP);
}


//  Checking whether Subscriber's data exists in Verification Database or not
int check(struct SubDB SubDB[],unsigned int SubNo,uint8_t technology) 
{
	int result = -1;
	for(int m = 0; m < LENGTH;m++) 
	{
	     if(SubDB[m].SubNo == SubNo && SubDB[m].technology == technology) 
	       {
			return SubDB[m].paidStatus;
		}
             else if (SubDB[m].SubNo == SubNo && SubDB[m].technology != technology)
                        return 2;
	}
	return result;
}

/*
262616161 04 1   	\\ Has access
262616162 05 1   	 \\Has access
262616163 02 1   	\\ Does not exit in Verification DB
262616164 05 1    	\\Does not exist
262616165 02 0   	\\ Not paid
262616166 03 0  	\\ Not paid
262616167 02 1   	\\Tech does not match
262616168 03 1    	\\ Tech does not match


262616161 04 1
262616162 05 1
262616165 02 0
262616166 03 0
262616167 04 1   	
262616168 05 1    	
*/
