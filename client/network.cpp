#include "network.h"
#include <boost/asio.hpp>  
#include <iostream>
#include <cstring> 
#include <string>

using namespace std;  // Using the std namespace

using boost::asio::ip::tcp;

// Declare the io_context and socket as global
boost::asio::io_context io_context;  // Updated from io_service to io_context
tcp::socket sock(io_context);



void connect_to_server(const std::string& server_ip, int server_port) {
    try {
        // Resolve the server address and port
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(server_ip, std::to_string(server_port));

        // Connect to the server
        boost::asio::connect(sock, endpoints);
        cout << "Connected to server at " << server_ip << ":" << server_port << endl;
    }
    catch (std::exception& e) {
        cerr << "Error connecting to server: " << e.what() << endl;
    }
}

void send_message(const std::string& message) {
    try {
        // Send the message to the server
        boost::asio::write(sock, boost::asio::buffer(message));
        cout << "Message sent: " << message << endl;
    }
    catch (std::exception& e) {
        cerr << "Error sending message: " << e.what() << endl;
    }
}

std::string receive_message() {
    try {
        char response[128];
        size_t length = sock.read_some(boost::asio::buffer(response));
        string received(response, length);
        return received;
    }
    catch (std::exception& e) {
        cerr << "Error receiving message: " << e.what() << endl;
        return "";
    }
}

void close_connection() {
    sock.close();  // Close the connection after use
    cout << "Connection closed." << endl;
}
