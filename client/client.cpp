/*
  This file contains the client-side logic, utilizing the
  network module to connect to the server, exchange data, and handle
  responses. It may also manage user interaction or other client
  application tasks.
*/
#include "client.h"
#include "config.h"
#include "network.h"
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "client_ui.h" 
#include <fstream>

using namespace std;  

string username;
// handle the different requests based on user input
void handle_request(int option, ClientSession& session) {
    if (option == 110) {  // register user option
        cout << "Enter username for registration: ";
        getline(cin, session.username);

        
        // create binary packet for registration request
        vector<uint8_t> packet = create_registration_packet(session.username, "");
        send_data(session.socket, packet);  // send registration request to server
    }
    else if (option == 150) {  // send message op
        string recipient, message;
        cout << "Enter recipient's username: ";
        getline(cin, recipient);
        cout << "Enter your message: ";
        getline(cin, message);

        // Create binary packet for sending message
        vector<uint8_t> packet = create_message_packet(recipient, message);
        send_data(session.socket, packet);  // Send message to server
    }
    else {
        display_err("Invalid option selected.");
    }
}

void handleResponse(ClientSession& session, const Response& resp) {
    const Header& h = resp.header;

    // For demonstration, convert client_id to hex or just print
    // but we might not always need it for a server response.
    cout << "Version: " << (int)h.version << "\n";
    cout << "Code:    " << h.code << "\n";
    cout << "Payload: " << h.payload_size << " bytes\n";

    switch (h.code) {
    case 2100: { // Registration success
        // Typically 16 bytes of new client ID in payload
        if (resp.payload.size() < 16) {
            cerr << "Error: incomplete client ID in payload\n";
            return;
        }

        string clientID(resp.payload.begin(), resp.payload.begin() + 16);
        cout << "Registration success! Client ID: " << clientID << "\n";

        // Save username and client ID to "my.info"
        ofstream myInfoFile("my.info", ios::app);
        if (myInfoFile.is_open()) {
            myInfoFile << session.username << " " << clientID << "\n";
            myInfoFile.close();
            cout << "Saved registration info to my.info\n";
        }
        else {
            cerr << "Error: Could not open my.info for writing.\n";
        }
        break;
    }
    case 2101: {
        // Possibly an error message in text form
        string errorMsg(resp.payload.begin(), resp.payload.end());
        cout << "Registration failed: " << errorMsg << "\n";
        break;
    }
             // Add more codes (2104, etc.) as needed
    default:
        std::cout << "Unknown response code: " << h.code << "\n";
        break;
    }
}


//sending input and receiving data from user
void client_function(const string& server_ip, int server_port) {
    try {
        boost::asio::io_context io_context;  // each client gets its own io_context
        ClientSession session(io_context);  // each client gets its own socket
        
        connect_to_server(session.socket, server_ip, server_port);

        // Send a message
        int usr_input = get_user_input();
        handle_request(usr_input, session.socket);

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
