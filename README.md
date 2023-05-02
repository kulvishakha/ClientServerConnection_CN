# ClientServerConnection_CN
Client Server Connection in Computer Network

This is the programming assignment which I did in Computer Network.

Program assignment 1:
Client using customized protocol on top of UDP protocol for sending information to the server. 
Client sends five packets to the server. The server acknowledges by sending ACK. The client will start an ack_timer of 3 secs at the time the packet is sent to server, if the ACK (Acknowledge) for each packet has not been received during ack_timer period by client before expiration of timer then client should retransmit the packet that was sent before. If the ACK for the packet does not arrive before the timeout, the client will retransmit the packet and restart the ack_timer, and the ack_timer should be reset for a total of 3 times. If no ACK was received from the server after resending the same packet 3 times, the client will response with message: “Server does not respond”.
Error handling is done for ACK and reject errors.


Program assignment 2:
Client using customized protocol on top of UDP protocol for requesting identification from server for access permission to the network.
The client requests access information from server; the server will verify the validity of the request and will respond accordingly.
The communication between client and server will use the ack_timer like assignment 1. Server checks whether Subscriber's data exists in Verification Database or not before giving access to the client.
