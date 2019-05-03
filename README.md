# CHATS

## Overview
CHATS is a multithreaded client server application. Once connected to the server, users by default join chat room and can instant message other users in the room.

Chats does not have any persistent state. Meaning that the user will only see messages from the time they entered a chat room to the point at which they leave. Consequently, chat history will not persist across sessions either - meaning that if a user leaves a room and rejoins later s/he won’t be able to see messages from the earlier session.

## Threading
In the Client side, CHATS uses two threads for sending and receiving messages respectively. On the Server side, CHATS uses two threads for each client socket for sending and receiving messages respectively. 

In CHATS, both the client and server are multi-threaded. 

The client has 2 threads:
A thread for sending messages to the server
A thread for receiving messages from the server.

Having separate threads for sending and receiving messages on the client avoids the problem of the user interface appearing to hang when receiving messages.

The server has 2 threads for each client socket:
A thread for sending messages to the client.
A thread for receiving messages from the client.

Having separate threads for each client socket allows CHATS to send messages to all clients concurrently. This avoids the latency that would be incurred if the server sent messages to clients one by one.

## Data Structures
Data Structures used in CHATS:
* A list of currently active users.
* A message struct containing the username of the sender and the message.
* A map for username to socket fd.
* A list of receive threads of server.
* A list of send threads of server.


