#ifndef CLIENT_H
#define CLIENT_H

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

// Function to connect to the server
void connect_to_server(tcp::socket& socket, const string& server_ip, int server_port);

// Function to send a message to the server
void send_data(tcp::socket& socket, const string& message);

// Function to receive a message from the server
string receive_data();

// Function to close the connection
void close_connection();

#endif  // CLIENT_H
