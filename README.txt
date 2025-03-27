README

Visual Studio 2019 (for C++ client)
Python 3.8.2 (for server)
Crypto++ compiled and linked (for client)

.exe file in \mmn15def\Debug

--overview:
This project implements a client-server communication system using a custom binary protocol over TCP. 
The server is written in Python, and the client is written in C++. 
The system allows multiple clients to connect to the server, register with a username, request the list of available users, and exchange messages with each other.

The protocol uses a fixed-format header and variable-length payloads. Each request and response is identified by a unique code. 
Data is sent and received as binary packets. 
The client application provides a simple user interface to interact with the server and manage user actions like registration, message sending, and message retrieval.

The server handles all incoming requests, stores user and message information in memory, and returns appropriate responses based on the protocol.

==========================
==========================

--Files Descriptions:
-->Client Side (C++)

client.cpp
This is the main entry point of the client. It initializes the client, connects to the server, handles user input, and manages the client loop for sending requests and receiving responses.

client.h
Defines the ClientSession class which stores the socket, username, client ID, and RSA keys. Also declares key client functions.

encryption.cpp / encryption.h
Handles hybrid encryption logic. Manages AES key generation, encryption of messages, and RSA encryption of symmetric keys. Uses AESWrapper and RSAWrapper.

AESWrapper.cpp / AESWrapper.h
Provides AES encryption and decryption functionality using the Crypto++ library. 

RSAWrapper.cpp / RSAWrapper.h
Handles RSA key generation, public/private key management, and encryption/decryption using RSA. Also based on Crypto++.

Base64Wrapper.cpp / Base64Wrapper.h
Wraps Base64 encoding and decoding. Used mainly to store or retrieve RSA keys in string format.

config.cpp / config.h
Loads the server IP and port from the server.info configuration file. 

network.cpp / network.h
Handles sending and receiving binary data over sockets. Contains logic to serialize/deserialize packet headers and payloads.

client_ui.cpp / client_ui.h
Contains utility functions for displaying prompts, menus, and error messages to the user.

utils.cpp / utils.h
helper functions

my.info
A local file used to store the client’s username and assigned client ID and public key after registration.

===========================

-->Server Side (Python)
server.py
Main server file. Opens a TCP socket, listens for incoming connections, and spawns threads to handle each client.

message_handler.py
Processes incoming requests from clients, such as registration, sending messages, and retrieving messages. Builds appropriate binary responses.

user_storage.py and user_manager.py
Stores registered users in memory. Provides functions to save users, check if a user exists, or retrieve a user by ID.

message_storage.py
In-memory storage for encrypted messages. Allows storing new messages and fetching them later by recipient ID.

protocolUtils.py
Handles binary packet construction and decoding. Defines the format of headers and payloads for each request/response type.

server.info
Configuration file containing the IP address and port the server should use.

=======================
=======================

--Incomplete Features:
Some planned features were not completed by the submission time, these include:

Message Encryption: The system was designed to use hybrid encryption (RSA for key exchange and AES for message encryption). 
Although the Crypto++ library and wrappers were integrated, full encryption flow was not finished.



