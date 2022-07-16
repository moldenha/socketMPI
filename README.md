# socketMPI
A super simple socket server based program that can function as a message passing interface

The way this works is that the head program sets up 2 servers. The first server is part of the message passing interface so nodes can connect to it. The second server is for testing purposes. The node sets up a client to connect to the head, and another server for testing purposes. The servers for testing purposes are pretty simple. You would have a client connect to the server running off the head and another to the server running off the node. If you send a message to the node server, the client connected to the head will recieve the message and vice versa. 
