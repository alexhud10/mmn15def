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
//requests to server - data to binary
//================================

vector<uint8_t> header_to_binary(const Header& header) {
    vector<uint8_t> binary_data;
    //manually placing into big endian order
    // add client ID
    binary_data.insert(binary_data.end(), begin(header.client_id), end(header.client_id));

    // add version
    binary_data.push_back(header.version);

    // add code (2 bytes)
    binary_data.push_back((header.code >> 8) & 0xFF);  // High byte
    binary_data.push_back(header.code & 0xFF);         // Low byte

    // add payload size (4 bytes)
    for (int i = 3; i >= 0; --i) {
        binary_data.push_back((header.payload_size >> (i * 8)) & 0xFF);
    }

    return binary_data;
}


vector<uint8_t> registration_payload_to_binary(const RegistrationPayload& payload) {
    vector<uint8_t> binary_data;

    //add name length and name
    binary_data.push_back(payload.name_length);
    binary_data.insert(binary_data.end(), payload.name.begin(), payload.name.end());

    // add public key length and public key
    binary_data.push_back(payload.public_key_length);
    binary_data.insert(binary_data.end(), payload.public_key.begin(), payload.public_key.end());

    return binary_data;
}

vector<uint8_t> message_payload_to_binary(const MessagePayload& payload) {
    vector<uint8_t> binary_data;
    // payload: recipient id, message type, content size, message
    
    // append recipient_id (16 bytes)
    binary_data.insert(binary_data.end(), payload.recipient_id, payload.recipient_id + 16);

    // append message_type (1 byte)
    binary_data.push_back(payload.message_type);

    // append content_size (4 bytes) in network byte order.
    // Convert content_size to network byte order.
    uint32_t cs_net = htonl(payload.content_size);
    binary_data.push_back((cs_net >> 24) & 0xFF);
    binary_data.push_back((cs_net >> 16) & 0xFF);
    binary_data.push_back((cs_net >> 8) & 0xFF);
    binary_data.push_back(cs_net & 0xFF);

    // append message content.
    binary_data.insert(binary_data.end(), payload.message_content.begin(), payload.message_content.end());

    return binary_data;
}

//===========================
// requests to server - create packet for each request
//===========================

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

vector<uint8_t> create_message_packet(const string& sender_id, const string& recipient, const string& message) {
    Header header;
    MessagePayload payload;
    string cid = sender_id;
    string rid = recipient;

    header.version = 1;
    header.code = 603;  
    memcpy(header.client_id, cid.data(), 16);

    payload.message_type = 3;  
    payload.message_content = message;
    payload.content_size = message.size();
    memcpy(payload.recipient_id, rid.data(), 16);

    
    vector<uint8_t> payload_binary = message_payload_to_binary(payload);

    header.payload_size = payload_binary.size();

    vector<uint8_t> packet = header_to_binary(header);

    packet.insert(packet.end(), payload_binary.begin(), payload_binary.end());
    return packet;
}

vector<uint8_t> create_get_users_packet(const string& id) {
    Header header;
    // Copy the stored client_id into the header's client_id field.
    // For requests, your header has 16 bytes for client_id.
    // Assume that if session.client_id is not 16 bytes, we pad or truncate accordingly.
    string cid = id;
    if (cid.size() < 16)
        cid.append(16 - cid.size(), '\0');
    else if (cid.size() > 16)
        cid = cid.substr(0, 16);
    memcpy(header.client_id, cid.data(), 16);

    header.version = 1;
    header.code = 601;  // get users list request code.
    header.payload_size = 0;  // no payload.

    // Build the packet from header only.
    vector<uint8_t> packet = header_to_binary(header);
    return packet;
}

//===========================
//response from server
//===========================

Response read_response(tcp::socket& socket) {
    // create buffer: read exactly the size of the Header (which is 1+2+4 = 23 bytes).
    vector<uint8_t> headerBuf(sizeof(ResponseHeader));
    boost::asio::read(socket, boost::asio::buffer(headerBuf.data(), headerBuf.size()));

    // copy the raw bytes into a Header struct
    ResponseHeader rawHeader;
    memcpy(&rawHeader, headerBuf.data(), sizeof(ResponseHeader)); //destination, source, number of bytes to copy 

    // convert endians
    rawHeader.code = ntohs(rawHeader.code);
    rawHeader.payload_size = ntohl(rawHeader.payload_size);

    // read the payload (if any)
    std::vector<uint8_t> payload;
    if (rawHeader.payload_size > 0) {
        cout << "payload is greater than 0" << "\n";
        payload.resize(rawHeader.payload_size);
        boost::asio::read(socket, boost::asio::buffer(payload.data(), payload.size()));
    }
    else {
        cout << "payload is size = 0" << "\n";
    }

    // create response - raw header and payload
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
        // resolve the server address and port
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
/*
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
}*/

void close_connection(tcp::socket& socket) {
    socket.close();  // Close the connection after use
    cout << "Connection closed." << endl;
}
