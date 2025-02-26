#pragma once
#ifndef NETWORK_H
#define NETWORK_H

#include <string>  // To handle strings for IP addresses and messages

// Function to establish a connection to the server
void connect_to_server(const std::string& server_ip, int server_port);

// Function to send a message to the server
void send_message(const std::string& message);

// Function to receive a response from the server
std::string receive_message();

// Function to close the socket connection after communication is done
void close_connection();

#endif  // NETWORK_H


