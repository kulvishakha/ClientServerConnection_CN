// Name       : Vishakha Kulkarni
// Student ID : 1632897

// Required Libraries
#include <stdio.h> //Standard I/O O/P
#include <stdlib.h> //For memory allocation, process control, conversions etc.
#include <string.h> //Contains macro definitions, constants, declarations of functions
#include <sys/types.h> //Defines data types used in the code 
#include <sys/socket.h>//Defines macros like CMSG_DATA, CMSG_NXTHDR to gain access to the data arrays
#include <netinet/in.h> //Defines IN6ADDR_LOOPBACK_INIT macro and it is defined by systsem
#include <netdb.h> //Defines macro IPPORT_RESERVED with the value of the highest reserved Internet port number
#include <arpa/inet.h> //Provides definations for internet operations
#include <sys/time.h> //Defines the timeval structure 

// Data Packet Header Format
struct Packet_HEADER
{
    unsigned short int packet_ID;     //Start of Packet Identifier - 0xFFFF
    unsigned char client_ID;          //Client Identifier - 0xFF (255 Decimal)
    unsigned short int data;          //Data Packet Type - 0XFFF1
    unsigned char segmentNumber;      // Sequence Number 0x0
    unsigned char length;             // length of payload - 0xFF;Max Length (255 Decimal)
} header = {0xFFFF, 0xFF, 0xFFF1, 0x0, 0xFF};

// Data Packet Ender Format
struct Packet_ENDER
{
    unsigned short int packet_endID;   //End of Packet Identifier - 0xFFFF
} ender = {0xFFFF};

// ACK Packet Format
struct ACK_PACKET
{
    unsigned short int packet_ID;      // Start of Packet Identifier - 0xFFFF
    unsigned char client_ID;           //Client Identifier - 0xFF (255 Decimal)
    unsigned short int ack;            //Acknoledgement Packet ID - 0xFFF2
    unsigned char segmentNumber;       //Sequence Number 0x0
    unsigned short int packet_endID;   //End of Packet Identifier - 0xFFFF
} ack = {0xFFFF, 0xFF, 0xFFF2, 0x0, 0xFFFF};

//REJECT Packet Format
struct Reject_PACKAGE
{
    unsigned short int packet_ID;       // Start of Packet Identifier - 0xFFFF
    unsigned char client_ID;            //Client Identifier - 0xFF (255 Decimal)
    unsigned short int reject;          //Reject ID - 0xFFF3
    unsigned short int rejectCode;      // Reject Code
    unsigned char segmentNumber;        //0x0
    unsigned short int packet_endID;    //End of Packet Identifier - 0xFFFF
} reject = {0xFFFF, 0xFF, 0xFFF3, 0xFFF4, 0x0, 0xFFFF};

//Defining Colors
#define RED "\x1B[31m"
#define YEL "\x1B[33m"
#define RESET "\x1B[0m"

// Displaying Packet Details
void printPacket_Info()
{
    printf(RED "\nPacket No: %d \n" RESET,header.segmentNumber);
    printf("Packet ID : \"%#X\"\n", header.packet_ID);
    printf("Client ID : \"%#X\"\n", header.client_ID);
    printf("DATA : \"%#X\"\n", header.data);
    printf("Sequence Number : \"%d\"\n", header.segmentNumber);
    printf("Length of payload : \"%#X\"\n", header.length);
    printf("End of Packet ID : \"%#X\"\n", ender.packet_endID);
    printf(RED "-----End of Packet -----\n" RESET);
}

// Displaying ACK Details
void printAck_Info()
{
    printf(RED "\nACK: %d\n" RESET, ack.segmentNumber);
    printf("Packet ID : \"%#X\"\n", ack.packet_ID);
    printf("Client ID : \"%#X\"\n", ack.client_ID);
    printf("ACK : \"%#X\"\n", ack.ack);
    printf("Received Sequence Number : \"%d\"\n", ack.segmentNumber);
    printf("End of Packet ID : \"%#X\"\n", ack.packet_endID);
    printf(RED "------------------------ End of ACK --------------------------------\n\n" RESET);
}

// Displaying REJECT Details
void printReject_Info()
{
    printf(RED "\nReject Packet: %d\n" RESET,reject.segmentNumber);
    printf("Packet ID : \"%#X\"\n", reject.packet_ID);
    printf("Client ID : \"%#X\"\n", reject.client_ID);
    printf("REJECT : \"%#X\"\n", reject.reject);
    printf("Reject sub code : \"%#X\"\n", reject.rejectCode);
    printf("Received Sequence Number : \"%d\"\n", reject.segmentNumber);
    printf("End of Packet ID : \"%#X\"\n", reject.packet_endID);
    printf(RED "----------------------- End of Reject Packet ------------------------\n\n" RESET);
}

//Store all packets in buffer;
char buffer[255],package[1024],response[16],payload[256];
int sock,n,packageNo,counter; 
//The SOCKADDR_IN structure defines a transport address and port for the AF_INET address family.
struct sockaddr_in server;
struct sockaddr_in from;
struct hostent *hp;
//Socket Timeout
//The struct timeval structure represents an elapsed time. 
struct timeval timeout;
//Defines variable with integral type of length of at least 32 bits.
socklen_t length;


// Packet Creation
void createPacket(int seg, char *payload)
{
    header.segmentNumber = seg;
    header.length = strlen(payload);
    //memset() is used to set all the bytes in a block of memory to a particular char value. 
    memset(package, 0, 264);
    //Copies the values of num bytes from the location pointed to by source directly to the memory block pointed to by destination
    memcpy(package, &header, sizeof(header));
    memcpy(package + sizeof(header), payload, strlen(payload));
    memcpy(package + sizeof(header) + strlen(payload), &ender, 2);
    printf(YEL "Packet is created:\n");
    printPacket_Info();
}

// Reject Packet Creation
void createReject_packet(unsigned short int code)
{
    reject.packet_ID = 0xFFFF;
    reject.client_ID = header.client_ID;
    reject.reject = 0xFFF3;
    reject.rejectCode = code;
    reject.segmentNumber = header.segmentNumber;
    reject.packet_endID = 0xFFFF;
}

//Error Message Creation
void errormsg(char *msg)
{
// The perror() function prints an error message to stderr . If string is not NULL and does not point to a null character, then string pointed to by string is printed to the standard error stream, followed by a colon and a space.
    perror(msg);
    exit(1);
}

// Send Packet Creation
void CreateSend_packet()
{
    counter = 3; // Timer setuo to 3
    while (counter >= 0)
    {
        //sending package to server
        // Termination of client operation after 3 tries
        if (counter == 3)
        {
            printf("\nSending message: %s\n", payload);
        }
        else
        {
            printf(YEL "\nSending message failed. Trying to resend");
            printf("\nSending message: %s\n", payload);
        }
        //Sending packet to the Server
        n = sendto(sock, package, sizeof(header) + strlen(payload) + 2, 0, (struct sockaddr *)&server, length);
        if (n < 0)
        {
            errormsg(YEL "Client Error: Failed to send the Packet\n");
        }

        //waiting for ACK
        //bzero()function is used to set all the socket structures with null values
        bzero(response, 16);
        //Receiving Data From the Server
        n = recvfrom(sock, response, 16, 0, (struct sockaddr *)&from, &length);
        if (n >= 0)
        {
            memcpy(&ack, response, sizeof(ack));
            if (ack.ack == 0xFFF2)
            {
                printf(YEL "INFO: Received an ACK\n " RESET); //Response from Server
                printAck_Info();
                break;
            }
            else
            {
                memcpy(&reject, response, sizeof(reject));
                printf(YEL "INFO: Received a reject Packet. " RESET);
                printf(YEL "Error Message: " RESET);
                if (reject.rejectCode == 0xFFF4)
                {
                    printf(YEL "Out of Sequence\n");  //Reject sub code - 0XFFF4
                }
                else if (reject.rejectCode == 0xFFF5)
                {
                    printf(YEL "Length Mismatch\n");   //Reject sub code - 0XFFF5
                }
                else if (reject.rejectCode == 0xFFF6)
                {
                    printf(YEL "End of Packet is missing\n"); //Reject sub code - 0XFFF6
                }
                else if (reject.rejectCode == 0xFFF7)
                {
                    printf(YEL "Duplicate Packet\n");    //Reject sub code - 0XFFF7
                }
                else
                {
                    printf(YEL "Unknown Error\n");
                }
                printReject_Info();
                counter++;
                break;
            }
        }
        counter--;
    }
    if (counter < 0)
    {
        errormsg(RED "Error: Server does not respond\n" RESET);
    }
}
