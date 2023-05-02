// Name       : Vishakha Kulkarni
// Student ID : 1632897

//Server Side Implementation

#include "headerfile.h"

int main(int argc, char *argv[])
{
    int sock,length,n;
    int counter = 0; //Initialize to 0
    //The SOCKADDR_IN structure specifies a transport address and port for the AF_INET address family.
    struct sockaddr_in server;
    struct sockaddr_in from;
    // Define variable with integral type of length of at least 32 bits.
    socklen_t fromlen;

    if (argc != 2)
    {
        fprintf(stderr, "Type: ./server port_number\n");
        exit(1);
    }

    //Socket Creation
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        errormsg("Error in Opening Socket");
    }
    length = sizeof(server);
    //bzero()function is used to set all the socket structures with null values
    bzero(&server, length);
    //sin_family. The address family for the transport address. This member should always be set to AF_INET.
    server.sin_family = AF_INET;
    //INADDR_ANY is a constant, that contain 0 in value . this will used only when you want connect from all active ports you don't care about ip-add .
    server.sin_addr.s_addr = INADDR_ANY;
     //The htons() function translates a short integer from host byte order to network byte order. 
    server.sin_port = htons(atoi(argv[1]));

    //Socket Binding with the server address
    //Bind the socket with the server address
    //The bind() function binds a unique local name to the socket with descriptor socket.
    if (bind(sock, (struct sockaddr *)&server, length) < 0)
    {
        errormsg("Error in Socket Binding Operation");
    }
    fromlen = sizeof(struct sockaddr_in);

    //Listening to client- incoming messages and sending ACKs
    printf(YEL "Server started successfully and now ready for incoming messages\n" RESET);
    while (1)
    {
        //Receieve the packet from the client
        n = recvfrom(sock, package, 1024, 0, (struct sockaddr *)&from, &fromlen);
        if (n < 0)
        {
            errormsg("recvfrom");
        }

        //Getting Header - 1st 7 bytes of Packet
        memcpy(&header, package, sizeof(header));

        //Getting Payload
        memcpy(&buffer, package + sizeof(header), n - sizeof(header) - 2);
        buffer[strlen(buffer)] = '\0';

        //Getting Ender - last 2 bytes pf Packet
        memcpy(&ender, package + n - 2, 2);

        // Packet Details
        printf(YEL "\n\nINFO: Received a packet: \n");
        printf("Packet number: %d \n", header.segmentNumber);
        printf("Packet size: %d\n", n);
        printf("Content: %s", buffer);
        printPacket_Info();
        printf("\n\n");

        //Error Handling
        if (header.segmentNumber == counter)
        {
            //Testing of Duplicate Packet
            printf(YEL "INFO: Packet is rejected by the Server \nError: Duplicate Packet Found\n" RESET);
            createReject_packet(0xFFF7);
            printReject_Info();
            //bzero()function is used to set all the socket structures with null values
            bzero(response, 16);
            //memset() is used to set all the bytes in a block of memory to a particular char value. 
            memset(response, 0, 16);
            //Copies the values of num bytes from the location pointed to by source directly to the memory block pointed to by destination
            memcpy(response, &reject, sizeof(reject));
            //Sending response packet to the client
            n = sendto(sock, &response, sizeof(reject), 0, (struct sockaddr *)&from, fromlen);
            if (n < 0)
            {
                errormsg(RED "INFO: Server has failed to send an ACK\n" RESET);
            }
        }
        else if (header.segmentNumber != counter + 1)
        {
            //Testing of Out of Sequence
            printf(YEL "INFO: Packet is rejected by the Server\nError: Out of Sequence\n" RESET);
            createReject_packet(0xFFF4);
            printReject_Info();
            bzero(response, 16);
            memset(response, 0, 16);
            memcpy(response, &reject, sizeof(reject));
            n = sendto(sock, &response, sizeof(reject), 0, (struct sockaddr *)&from, fromlen);
            if (n < 0)
            {
                errormsg(RED "INFO: Server has failed to send an ACK\n" RESET);
            }
        }
        else if (header.length != strlen(buffer))
        {
            //Testing of Length Mismatch
            printf(YEL "INFO: Packet is rejected by the Server\nError: Length Mismatch\n" RESET);
            createReject_packet(0xFFF5);
            printReject_Info();
            bzero(response, 16);
            memset(response, 0, 16);
            memcpy(response, &reject, sizeof(reject));
            n = sendto(sock, &response, sizeof(reject), 0, (struct sockaddr *)&from, fromlen);
            if (n < 0)
            {
                errormsg(RED "INFO: Server has failed to send an ACK\n" RESET);
            }
        }
        else if (ender.packet_endID != 0xFFFF)
        {
            //Testing of End of Packet Missing
            printf(YEL "INFO: Packet is rejected by the Server\nError: End of Packet is missing\n" RESET);
            createReject_packet(0xFFF6);
            printReject_Info();
            bzero(response, 16);
            memset(response, 0, 16);
            memcpy(response, &reject, sizeof(reject));
            n = sendto(sock,&response,sizeof(reject),0,(struct sockaddr*)&from,fromlen);
            if (n < 0)
            {
                errormsg(RED "INFO: Server has failed to send an ACK\n" RESET);
            }
        }
        else
        {
            printf(YEL "INFO: Packet is accepted by the Server \nSending an ACK\n" RESET);
            counter++;
            ack.packet_ID = 0xFFFF;
            ack.client_ID = header.client_ID;
            ack.ack = 0xFFF2;
            ack.segmentNumber = header.segmentNumber;
            ack.packet_endID = 0xFFFF;
            printf("sizeof(ack) == %lu\n", sizeof(ack));
            printAck_Info();
            bzero(response, 16);
            memset(response, 0, 16);
            memcpy(response, &ack, sizeof(ack));
            n = sendto(sock, &response, sizeof(ack), 0, (struct sockaddr *)&from, fromlen);
            if (n < 0)
            {
                errormsg(RED "INFO: Server has failed to send an ACK\n" RESET);
            }

            //Reset the server counter for testing purposes
            if (counter == 5)
            {
            printf(YEL "\n\nReseting the packet counter\n\n");
            counter = 0;
            }
        }
    }
}
