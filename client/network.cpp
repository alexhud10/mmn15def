/*
This source file implements the network-related functions
 using Boost.Asio to manage socket connections
 and data transfer between the client and the server
*/


#include "network.h"
#include <boost/asio.hpp>  
#include <iostream>
#include <cstring> 
#include <string>


using namespace std;  // Using the std namespace

using boost::asio::ip::tcp;

// Declare the io_context and socket as global
boost::asio::io_context io_context; 

//================================
//requests to server
//================================

vector<uint8_t> header_to_binary(const Header& header) {
    vector<uint8_t> binary_data;
    //manually placing into big endian order
    // Add client ID
    binary_data.insert(binary_data.end(), begin(header.client_id), end(header.client_id));

    // Add version
    binary_data.push_back(header.version);

    // Add code (2 bytes)
    binary_data.push_back((header.code >> 8) & 0xFF);  // High byte
    binary_data.push_back(header.code & 0xFF);         // Low byte

    // Add payload size (4 bytes)
    for (int i = 3; i >= 0; --i) {
        binary_data.push_back((header.payload_size >> (i * 8)) & 0xFF);
    }

    return binary_data;
}


vector<uint8_t> registration_payload_to_binary(const RegistrationPayload& payload) {
    vector<uint8_t> binary_data;

    // Add name length and name
    binary_data.push_back(payload.name_length);
    binary_data.insert(binary_data.end(), payload.name.begin(), payload.name.end());

    // Add public key length and public key
    binary_data.push_back(payload.public_key_length);
    binary_data.insert(binary_data.end(), payload.public_key.begin(), payload.public_key.end());

    return binary_data;
}

vector<uint8_t> message_payload_to_binary(const MessagePayload& payload) {
    vector<uint8_t> binary_data;

    // Add recipient length and recipient name
    binary_data.push_back(payload.recipient_length);
    binary_data.insert(binary_data.end(), payload.recipient.begin(), payload.recipient.end());

    // Add message type length and message type
    binary_data.push_back(payload.message_type_length);
    binary_data.insert(binary_data.end(), payload.message_type.begin(), payload.message_type.end());

    // Add content size (4 bytes)
    for (int i = 3; i >= 0; --i) {
        binary_data.push_back((payload.content_size >> (i * 8)) & 0xFF);
    }

    // Add message content
    binary_data.insert(binary_data.end(), payload.content.begin(), payload.content.end());

    return binary_data;
}

vector<uint8_t> create_registration_packet(const string& username, const std::string& public_key) {
    Header header;
    RegistrationPayload payload;

    // header information
    std::array<uint8_t, 16> client_id = { 0 };  // Zero-initialized for now
    header.version = 1;
    header.code = 600;  // Registration code
    header.payload_size = sizeof(payload.name_length) + username.size() + sizeof(payload.public_key_length) + public_key.size();

    // payload information
    payload.name = username;
    payload.name_length = username.size();
    payload.public_key = public_key;
    payload.public_key_length = public_key.size();

    // Convert header and payload to binary
    vector<uint8_t> packet = header_to_binary(header);
    vector<uint8_t> payload_binary = registration_payload_to_binary(payload);

    // Append payload to packet
    packet.insert(packet.end(), payload_binary.begin(), payload_binary.end());

    return packet;
}

vector<uint8_t> create_message_packet(const string& recipient, const string& message) {
    Header header;
    MessagePayload payload;

    header.version = 1;
    header.code = 603;  // Message request code
    header.payload_size = sizeof(payload.recipient_length) + recipient.size() + sizeof(payload.message_type_length) + payload.message_type.size() + sizeof(payload.content_size) + message.size();

    payload.recipient = recipient;
    payload.recipient_length = recipient.size();
    payload.message_type = "text";  // Example: Message type
    payload.message_type_length = payload.message_type.size();
    payload.content = message;
    payload.content_size = message.size();

    vector<uint8_t> packet = header_to_binary(header);
    vector<uint8_t> payload_binary = message_payload_to_binary(payload);

    packet.insert(packet.end(), payload_binary.begin(), payload_binary.end());
    return packet;
}

//===========================
//response from server
//===========================

Response readResponse(tcp::socket& socket) {
    // 1) First, read exactly the size of the Header (which is 16+1+2+4 = 23 bytes).
    //    But let's store it in a temporary buffer for correct endianness conversion.
    vector<uint8_t> headerBuf(sizeof(Header));
    boost::asio::read(socket, boost::asio::buffer(headerBuf.data(), headerBuf.size()));

    // 2) Copy the raw bytes into a Header struct
    Header rawHeader;
    memcpy(&rawHeader, headerBuf.data(), sizeof(Header)); //destination, source, number of bytes to copy 

    // 3) Convert from network byte order to host byte order where needed.
    //    client_id is just 16 bytes, no endianness. version is 1 byte, also no endianness.
    rawHeader.code = ntohs(rawHeader.code);
    rawHeader.payload_size = ntohl(rawHeader.payload_size);

    // 4) Now read the payload (if any)
    std::vector<uint8_t> payload;
    if (rawHeader.payload_size > 0) {
        payload.resize(rawHeader.payload_size);
        boost::asio::read(socket, boost::asio::buffer(payload.data(), payload.size()));
    }

    // 5) Construct a DecodedResponse
    Response resp;
    resp.header = rawHeader;
    resp.payload = std::move(payload);
    return resp;
}





//===========================
// connection to server
//===========================

void connect_to_server(tcp::socket& socket, const string& server_ip, int server_port) {
    try {
        // Resolve the server address and port
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(server_ip, to_string(server_port));

        // Connect to the server
        boost::asio::connect(socket, endpoints);
        cout << "Connected to server at " << server_ip << ":" << server_port << endl;
    }
    catch (exception& e) {
        cerr << "Error connecting to server: " << e.what() << endl;
    }
}

void send_data(tcp::socket& socket, const vector<uint8_t>& data) {
    try {
        // send the binary info to the server
        boost::asio::write(socket, boost::asio::buffer(data.data(), data.size()));
        cout << "sent: " << data.size() << " bytes of data" << endl;
    }
    catch (exception& e) {
        cerr << "Error sending data: " << e.what() << endl;
    }
}

string receive_data(tcp::socket& socket) {
    try {
        char response[128];
        size_t length = socket.read_some(boost::asio::buffer(response));
        string received(response, length);
        return received;
    }
    catch (std::exception& e) {
        cerr << "Error receiving message: " << e.what() << endl;
        return "";
    }
}

void close_connection(tcp::socket& socket) {
    socket.close();  // Close the connection after use
    cout << "Connection closed." << endl;
}
