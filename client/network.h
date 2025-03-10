#pragma once
#ifndef NETWORK_H
#define NETWORK_H

#include "network.h"
#include <boost/asio.hpp>  
#include <iostream>
#include <cstring> 
#include <string>
#include <boost/asio.hpp>  
#include <vector>

using boost::asio::ip::tcp;


//header
struct Header {
    uint8_t client_id[16];  // 16 bytes: Client ID
    uint8_t version;        // 1 byte: Version (e.g., 1)
    uint16_t code;          // 2 bytes: Code for request type (e.g., 600, 603)
    uint32_t payload_size;  // 4 bytes: Size of the payload
};

//code 600 - registration
struct RegistrationPayload {
    uint8_t name_length;      // Length of the name (1 byte)
    std::string name;         // Name of the user (up to 255 characters)
    uint8_t public_key_length; // Length of public key (1 byte)
    std::string public_key;   // Public key (up to 160 characters)
};

//code 603 - send message 
struct MessagePayload {
    uint8_t recipient_length;  // Length of the recipient's name
    std::string recipient;     // Recipient's name
    uint8_t message_type_length;  // Length of the message type
    std::string message_type;     // Type of the message (e.g., "text")
    uint32_t content_size;         // Size of the message content
    std::string content;           // Message content
};

struct ResponseHeader {
    uint8_t  version;
    uint16_t code;          // e.g. 2100
    uint32_t payload_size;  // size of the payload
};

struct Response {
    Header header;
    std::vector<uint8_t> payload;
};


std::vector<uint8_t> header_to_binary(const Header& header);

std::vector<uint8_t> registration_payload_to_binary(const RegistrationPayload& payload);

std::vector<uint8_t> message_payload_to_binary(const MessagePayload& payload);

std::vector<uint8_t> create_registration_packet(const std::string& username, const std::string& public_key);

std::vector<uint8_t> create_message_packet(const std::string& recipient, const std::string& message);

Response readResponse(tcp::socket& socket);

// Function to establish a connection to the server
void connect_to_server(tcp::socket& socket, const std::string& server_ip, int server_port);

// Function to send a message to the server
void send_data(tcp::socket& socket, const std::vector<uint8_t>& data);

// Function to receive a response from the server
std::string receive_data(tcp::socket& socket);

// Function to close the socket connection after communication is done
void close_connection(tcp::socket& socket);

#endif  // NETWORK_H


