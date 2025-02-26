#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "config.h"
#include "network.h"  // For network functions like connect_to_server, send_message, etc.

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
void connect_to_server(const string& server_ip, int server_port);

// Function to send a message to the server
void send_message(const string& message);

// Function to receive a message from the server
string receive_message();

// Function to close the connection
void close_connection();

#endif  // CLIENT_H
