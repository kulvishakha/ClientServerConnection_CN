Programming Assignment 1: Client using customized protocol on top of UDP protocol for sending information to the server. This folder contains source code,
PA1_Output and readme file.

The files in this folder are as follows:
1. client.c
2. server.c
3. readme.txt
4. PA1_Output.pdf
5. client (binary)
6. server (binary)

Follow the below steps for Program Compilation: 

1. Launch the terminal and execute the command following commands,server needs to be started 1st:
     a) gcc -o server server.c             //To compile server.c file.   
     b) ./server 1626                      //To start the server

2. Open another terminal and execute the following commands:
     a) gcc -o  client client.c            //To compile client.c file.   
     b) ./client localhost 1626            //To start the client; Format - ./client <host name> <port number>


After client execution, the packets will be transferred.

Note: -o in gcc command generates binary file with the name you provided as an argument. In this case it is -o server and -o client.