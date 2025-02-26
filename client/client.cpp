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
#include <vector>

using namespace std;  

void send_message(tcp::socket& socket, const string& message) {
    send_data(socket, message);  // function from network.cpp
}

string receive_message(tcp::socket& socket) {
    return receive_data(socket);  // function from network.cpp
}

void client_function(int client_id, const string& server_ip, int server_port) {
    try {
        boost::asio::io_context io_context;  // each client gets its own io_context
        tcp::socket sock(io_context);  // each client gets its own socket
        
        connect_to_server(sock, server_ip, server_port);

        // Send a message
        string message = "Hello from Client " + to_string(client_id) + "\n";
        send_message(sock, message);

        // Receive a response
        string response = receive_message(sock);
        if (!response.empty()) {
            cout << "Client " << client_id << " received: " << response << endl;
        }
        else {
            cout << "No response from server." << endl;
        }

        // Close the connection
        close_connection(sock);
    }
    catch (const std::exception& e) {
        cerr << "Client " << client_id << " encountered an error: " << e.what() << endl;
    }
}



int main() {
    
    
    // initialize the config object and load server info from server.info
    config cfg;
    cfg.loadFile("server.info");

    // get the server IP and port from the config object
    string server_ip = cfg.getIP();
    int server_port = cfg.getPort();

    // number of client threads to create
    const int num_clients = 2;

    // create and launch multiple client threads
    vector<thread> client_threads;

    for (int i = 1; i <= num_clients; ++i) {
        client_threads.push_back(thread(client_function, i, server_ip, server_port));
    }


    // wait for all client threads to finish
    for (auto& t : client_threads) {
        t.join();
    }

    
    

    return 0;
}
