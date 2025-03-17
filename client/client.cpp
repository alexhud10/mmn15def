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
#include <functional>
#include <thread>

using namespace std;  


// handle the different requests based on user input
void handle_request(int option, ClientSession& session) {
    if (option == 110) {  // register user option
        string username;
        cout << "Enter username for registration: ";
        getline(cin, session.username);

        
        // create binary packet for registration request
        vector<uint8_t> packet = create_registration_packet(session.username, "");
        send_data(session.socket, packet);  // send registration request to server
    }
    else if (option == 120) { //users list
        if (session.client_id.empty()) {
            cerr << "Error: Client ID is not set. Please register first.\n";
            return;
        }
        vector<uint8_t> packet = create_get_users_packet(session.client_id);
        send_data(session.socket, packet);

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

// getting response(raw header and payload) from read_response function and handle it
void handle_response(ClientSession& session, const Response& resp) {
    const ResponseHeader& h = resp.header;

    // For demonstration, convert client_id to hex or just print
    cout << "Version: " << (int)h.version << "\n";
    cout << "Code:    " << h.code << "\n";
    cout << "Payload: " << h.payload_size << " bytes\n";

    switch (h.code) {
    case 2100: { // Registration success
        // 16 bytes of new client ID in payload
        if (resp.payload.size() < 16) {
            cerr << "Error: incomplete client ID in payload\n";
            return;
        }

        string clientID(resp.payload.begin(), resp.payload.begin() + 16);
        cout << "Registration success! Client ID: " << clientID << "\n";
        // add user id to session
        session.client_id = clientID;
        // save username and client ID to "my.info"
        ofstream myInfoFile("my.info", ios::app);
        if (myInfoFile.is_open()) {
            myInfoFile << session.username << "\n" << clientID << "\n";
            myInfoFile.close();
            cout << "Saved registration info to my.info with id " << session.client_id << "\n";
        }
        else {
            cerr << "Error: Could not open my.info for writing.\n";
        }
        break;
    }
    case 2101: {
        // Each user record is 16 bytes for user_id + 255 bytes for username = 271 bytes.
        size_t record_size = 16 + 255; // 271 bytes per record.
        if (resp.payload.size() % record_size != 0) {
            cerr << "Error: Payload size is not a multiple of " << record_size << " bytes.\n";
            return;
        }
        size_t num_users = resp.payload.size() / record_size;
        vector<string> user_list;
        for (size_t i = 0; i < num_users; i++) {
            size_t offset = i * record_size;
            // Skip the first 16 bytes (user ID), and read the next 255 bytes for username.
            string username(resp.payload.begin() + offset + 16,
                resp.payload.begin() + offset + record_size);
            // Trim off trailing null characters.
            size_t null_pos = username.find('\0');
            if (null_pos != string::npos) {
                username = username.substr(0, null_pos);
            }
            user_list.push_back(username);
        }
        // Call a UI function to display the user list.
        cout << "for user: " << session.client_id << " display users list" << "\n";
        display_user_list(user_list);  // Assuming this function is defined in your client UI module.
        break;
    }
    case 2106: {
        // Possibly an error message in text form
        string errorMsg(resp.payload.begin(), resp.payload.end());
        cout << "Registration failed: " << errorMsg << "\n";
        break;
    }
             // Add more codes (2104, etc.) as needed
    default:
        cout << "Unknown response code: " << h.code << "\n";
        break;
    }
}


//sending input and receiving data from user
void client_function(const string& server_ip, int server_port, ClientSession &session) {
    try {
        
        while (true) {
            /*
            boost::asio::io_context io_context;  // each client gets its own io_context
            ClientSession session(io_context);  // each client gets its own socket
            */
            connect_to_server(session.socket, server_ip, server_port);
            // Send a message
            int usr_input = get_user_input();

            handle_request(usr_input, session);

            Response resp = read_response(session.socket);
            cout << "response from server was read ... " << "\n";
            handle_response(session, resp);
            cout << "response handled successfuly " << "\n";
        }
    }
    catch (const std::exception& e) {
        display_err("Client encountered an error: " + string(e.what()));
    }
}



int main() {

    // initialize the config object and load server info from server.info
    config cfg;
    cfg.load_file("server.info");

    // get the server IP and port from the config object
    string server_ip = cfg.get_ip();
    int server_port = cfg.get_port();


    // create and launch multiple client threads
    vector<thread> client_threads;

    boost::asio::io_context io_context;  // each client gets its own io_context
    ClientSession session(io_context);  // each client gets its own socket

    //client_threads.push_back(thread(client_function, server_ip, server_port, std::ref(session)));
    client_threads.push_back(std::thread([&]() {client_function(server_ip, server_port, session);
        }));

    // wait for all client threads to finish
    for (auto& t : client_threads) {
        t.join();
    }

    
    

    return 0;
}
