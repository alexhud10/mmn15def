#pragma once
#ifndef NETWORK_H
#define NETWORK_H

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
    uint8_t recipient_id[16];  // recipient 
    uint8_t message_type;     // 1 byte
    uint32_t content_size;         // 4 bytes
    std::string message_content;   // message content
};


#pragma pack(push, 1)
struct ResponseHeader {
    uint8_t  version;       //1 byte
    uint16_t code;          // 2 byte
    uint32_t payload_size;  // size of the payload - 4 byte
};
#pragma pack(pop)

struct Response {
    ResponseHeader header;
    std::vector<uint8_t> payload;
};


std::vector<uint8_t> header_to_binary(const Header& header);

std::vector<uint8_t> registration_payload_to_binary(const RegistrationPayload& payload);

std::vector<uint8_t> message_payload_to_binary(const MessagePayload& payload);

std::vector<uint8_t> create_registration_packet(const std::string& username, const std::string& public_key);

std::vector<uint8_t> create_message_packet(const std::string& sender_id, const std::string& recipient, const std::string& message);

std::vector<uint8_t> create_pull_messages_packet(const std::string& client_id);

std::vector<uint8_t> create_get_users_packet(const std::string& id);

Response read_response(tcp::socket& socket);

void connect_to_server(tcp::socket& socket, const std::string& server_ip, int server_port);

void send_data(tcp::socket& socket, const std::vector<uint8_t>& data);

//std::string receive_data(tcp::socket& socket);

void close_connection(tcp::socket& socket);

#endif  // NETWORK_H


