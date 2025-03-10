#ifndef CLIENT_H
#define CLIENT_H
#pragma once
#include <boost/asio.hpp>  
#include <string>
#include "config.h"
#include "network.h"  // For network functions like connect_to_server, send_message, etc.
using boost::asio::ip::tcp;
using namespace std;

// Declare the configuration class to manage server IP and port
class config {
public:
    // Constructor with default values
    config();

    // Method to load server information from a file
    void loadFile(const string& filename);

    // Getter for the server IP
    string getIP() const;

    // Getter for the server port
    int getPort() const;

private:
    string serverIP;
    int serverPort;
};

class ClientSession {
public:
    std::string username; // Holds the username entered by the user.
    tcp::socket socket;

    // Constructor: initializes the socket with the io_context.
    ClientSession(boost::asio::io_context& io_context)
        : socket(io_context) {}
};

void handle_request(int option, tcp::socket& socket);

void handleResponse(const Response& resp);

void client_function(const string& server_ip, int server_port);

#endif  // CLIENT_H
