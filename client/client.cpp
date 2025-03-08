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
#include "client_ui.h" 
#include <random>

using namespace std;  

// handle the different requests based on user input
void handle_request(int option, tcp::socket& socket) {
    if (option == 110) {  // register user option
        string username;
        cout << "Enter username for registration: ";
        getline(cin, username);

        
        // create binary packet for registration request
        vector<uint8_t> packet = create_registration_packet(username, "");
        send_data(socket, packet);  // send registration request to server
    }
    else if (option == 150) {  // send message op
        string recipient, message;
        cout << "Enter recipient's username: ";
        getline(cin, recipient);
        cout << "Enter your message: ";
        getline(cin, message);

        // Create binary packet for sending message
        vector<uint8_t> packet = create_message_packet(recipient, message);
        send_data(socket, packet);  // Send message to server
    }
    else {
        display_err("Invalid option selected.");
    }
}
/*
void send_message(tcp::socket& socket, const string& message) {
    send_data(socket, message);  // function from network.cpp
}

string receive_message(tcp::socket& socket) {
    return receive_data(socket);  // function from network.cpp
}

*/

//sending input and receiving data from user
void client_function(const string& server_ip, int server_port) {
    try {
        boost::asio::io_context io_context;  // each client gets its own io_context
        tcp::socket sock(io_context);  // each client gets its own socket
        
        connect_to_server(sock, server_ip, server_port);

        // Send a message
        int usr_input = get_user_input();
        handle_request(usr_input, sock);
        /*
        // Receive a response
        string response = receive_data(sock);
        if (!response.empty()) {
            display_message(response);
        }
        else {
            display_err("No response from server.");
        }

        // Close the connection
        close_connection(sock);*/
    }
    catch (const std::exception& e) {
        display_err("Client encountered an error: " + string(e.what()));
    }
}



int main() {

    // initialize the config object and load server info from server.info
    config cfg;
    cfg.loadFile("server.info");

    // get the server IP and port from the config object
    string server_ip = cfg.getIP();
    int server_port = cfg.getPort();


    // create and launch multiple client threads
    vector<thread> client_threads;

    client_threads.push_back(thread(client_function, server_ip, server_port));
    


    // wait for all client threads to finish
    for (auto& t : client_threads) {
        t.join();
    }

    
    

    return 0;
}
