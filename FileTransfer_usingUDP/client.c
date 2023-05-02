// Name       : Vishakha Kulkarni
// Student ID : 1632897

//Client Side Implementation

#include "headerfile.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Type: ./client server_name port_number(localhost 8080)\n");
        exit(1);
    }

    //Socket File descriptor Creation for socket communication - Socket Initialization
    //AF_INET ensures that client and server are local
    //SOCK_DGRAM for connection less services
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        errormsg(RED "Client Error: Socket Creation Failure\n");
    //This function causes an immediate termination of the entire program done by the operation system.
    }
    //sin_family. The address family for the transport address
    server.sin_family = AF_INET;
    //To retrieve information about a host
    hp = gethostbyname(argv[1]);
    if (hp == 0)
    {
        errormsg(RED "Client Error: Unknown Host\n");
    }
    
    //The bcopy() function copies n bytes from source to destination
    bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
    //The htons() function translates a short integer from host byte order to network byte order. 
    server.sin_port = htons(atoi(argv[2]));
    
    //To wait a whole number of seconds,just set tv_sec
    timeout.tv_sec = 3;
    //To wait a portion of a second,just set tv_usec.
    timeout.tv_usec = 0;

    //The setsockopt() function provides an application program with the means to control socket behavior.
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    
    //Getting the size 
    length = sizeof(struct sockaddr_in);

    //Sending 5 Error-free packets
    printf(YEL "\n\nTest1: Send 5 Error-free packets\n\n\n" RESET);
    packageNo = 1;
    while(packageNo <= 5)
    {
    //Packet Creation
    //bzero()function is used to set all the socket structures with null values
    bzero(payload, 256);
    //Copies the values of num bytes from the location pointed to by source directly to the memory block pointed to by destination
    memcpy(payload, "Payload of a package.\n", 34);
    //Creating packets
    createPacket(packageNo, payload);
    //Sending packets to the server
    CreateSend_packet();
    packageNo++;
    }
    printf(YEL "\n\nReseting the packet counter for Test2\n\n" RESET);

    printf(YEL "\n\nTest2: Sending 5 packets- 1 Correct and 4 error packets\n\n" RESET);

    //Packet 1 : correct Packet
    printf(YEL "Packet 1: Correct Packet\n\n" RESET);
    bzero(payload, 256);
    memcpy(payload, "Payload of a package.\n", 34);
    createPacket(1, payload);
    CreateSend_packet();

    //Packet 2: Testing of Duplicate Error;Reject sub code - 0XFFF7
    //Checking the identical copies of packet.
    printf(YEL "Packet 2: Testing Duplicate Packet Error\n\n" RESET);
    bzero(payload, 256);
    memcpy(payload, "Payload of a package.\n", 34);
    createPacket(1, payload);
    CreateSend_packet();

    //Packet 3:Testing Out of Sequence Error;Reject sub code - 0XFFF4
    printf(YEL "Packet 3: Testing out of Sequence Error\n\n" RESET);
    bzero(payload, 256);
    memcpy(payload, "Payload of a package.\n", 34);
    createPacket(3, payload);
    CreateSend_packet();

    //Packet 4: Testing Length Mismatch Error;Reject sub code - 0XFFF5
    printf(YEL "Packet 4: Testing length Mismatch Error\n\n" RESET);

    // Creating an Packet with Error
    bzero(payload, 256);
    memcpy(payload, "Payload of a package.\n", 34);
    header.segmentNumber = 2;
    header.length = 0x44;
    //memset() is used to set all the bytes in a block of memory to a particular char value. 
    memset(package, 0, 264);
    memcpy(package, &header, sizeof(header));
    memcpy(package + sizeof(header), payload, strlen(payload));
    memcpy(package + sizeof(header) + strlen(payload), &ender, 2);
    printf(YEL "Packet is created:\n");
    printPacket_Info();
    CreateSend_packet();

    //Packet 5:Testing end of packet missing error;Reject sub code - 0XFFF6
    printf(YEL "Packet 5: Testing end of Packet missing error\n\n" RESET);
    bzero(payload, 256);
    memcpy(payload, "Payload of a package.\n", 34);
    createPacket(2, payload);
    ender.packet_endID = 0xFFF0;
    memcpy(package + sizeof(header) + strlen(payload), &ender, 2);
    printf(YEL "\nPacket is altered: End of Packet ID changed\n " RESET);
    printPacket_Info();
    CreateSend_packet();
    int close(int sock);

}
