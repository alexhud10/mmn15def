#pragma once
#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <boost/asio.hpp>  

using boost::asio::ip::tcp;
// Function to establish a connection to the server
void connect_to_server(tcp::socket& socket, const std::string& server_ip, int server_port);

// Function to send a message to the server
void send_data(tcp::socket& socket, const std::string& message);

// Function to receive a response from the server
std::string receive_data(tcp::socket& socket);

// Function to close the socket connection after communication is done
void close_connection(tcp::socket& socket);

#endif  // NETWORK_H


