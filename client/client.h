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
/*class config {
public:
    // Constructor with default values
    config();

    // Method to load server information from a file
    void load_file(const string& filename);

    // Getter for the server IP
    string get_ip() const;

    // Getter for the server port
    int get_port() const;

private:
    string serverIP;
    int serverPort;
};*/

class ClientSession {
public:
    std::string username; // holds the username entered by the user.
    std::string client_id; // holds id the assigned from registration
    tcp::socket socket;

    // constructor: initializes the socket with the io_context.
    ClientSession(boost::asio::io_context& io_context)
        : socket(io_context) {}
};

void handle_request(int option, ClientSession& session);

void handle_response(ClientSession& session, const Response& resp);

void client_function(const string& server_ip, int server_port);

#endif  // CLIENT_H
