/*
  This file contains the client-side logic, utilizing the
  network module to connect to the server, exchange data, and handle
  responses. It may also manage user interaction or other client
  application tasks.
*/

#include "config.h"
#include "network.h"
#include <boost/asio.hpp>
#include <iostream>
#include <string>

using namespace std;  

int main() {
    
    
    // initialize the config object and load server info from server.info
    config cfg;
    cfg.loadFile("server.info");

    // get the server IP and port from the config object
    string server_ip = cfg.getIP();
    int server_port = cfg.getPort();

    // print the server IP and port
    cout << "will connect to server at " << server_ip << ":" << server_port << endl;

    // step 1: connect to the server using the loaded IP and port
    connect_to_server(server_ip, server_port);

    // step 2: send a test message to the server
    string message = "Hello from the client!";
    send_message(message);

    // step 3: receive the server's response
    string response = receive_message();
    if (!response.empty()) {
        cout << "Received from server: " << response << endl;
    }
    else {
        cout << "No response from server." << endl;
    }

    // close the connection after use
    close_connection();
    

    return 0;
}
