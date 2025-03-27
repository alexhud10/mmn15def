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
#include <cstddef>
#include "utils.h"
#include "Base64Wrapper.h"


using namespace std;  


// handle the different requests based on user input
void handle_request(int option, ClientSession& session) {
    if (option == 110) {  // register user option
        string username;
        cout << "Enter username for registration: ";
        getline(cin, session.username);
        bool existing_id = user_in_file(session.username);

        if (existing_id == true) {
            cout << "User already registered "<< endl;
            return;
        }
        
        // generate key pair
        session.rsaPrivate = std::make_unique<RSAPrivateWrapper>();
        session.rsaPrivateKey = Base64Wrapper::encode(session.rsaPrivate->getPrivateKey());
        session.rsaPublicKey = session.rsaPrivate->getPublicKey();

        string base64_public_key = Base64Wrapper::encode(session.rsaPublicKey);
        // create binary packet for registration request
        vector<uint8_t> packet = create_registration_packet(session.username, base64_public_key);
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
    else if (option == 140) {  // get waiting messages
        if (session.client_id.empty()) {
            cerr << "Error: Client ID is not set. Please register first.\n";
            return;
        }
        vector<uint8_t> packet = create_pull_messages_packet(session.client_id);
        send_data(session.socket, packet);
    }
    else if (option == 150) {  // send message op
        if (session.client_id.empty()) {
            cerr << "Error: Client ID is not set. Please register first.\n";
            return;
        }
        string recipient, message;
        cout << "Enter recipient's username: " << endl;
        getline(cin, recipient);
        string recipient_id = get_id_by_username(recipient);
        if (recipient_id.empty()) {
            display_err("Recipient not found in local info");
            return;
        }
        cout << "Enter your message: " << endl;
        getline(cin, message);

        // binary packet for sending message
        vector<uint8_t> packet = create_message_packet(session.client_id, recipient_id, message);
        send_data(session.socket, packet);  // Send message to server
    }
    else if (option == 0) {
        cout << "Exiting client. Releasing resources..." << endl;
        close_connection(session.socket);  
        exit(0);  // terminate the program
    }
    else {
        display_err("Invalid option selected.");
    }
}

// getting response(raw header and payload) from read_response function and handle it
void handle_response(ClientSession& session, const Response& resp) {
    const ResponseHeader& h = resp.header;

    // for demonstration, convert client_id to hex or just print
    //cout << "Version: " << (int)h.version << "\n";
    //cout << "Code:    " << h.code << "\n";
    //cout << "Payload: " << h.payload_size << " bytes\n";

    switch (h.code) {
    case 2100: { // registration success
        // 16 bytes of new client ID in payload
        if (resp.payload.size() < 16) {
            cerr << "Error: incomplete client ID in payload\n";
            return;
        }

        string clientID(resp.payload.begin(), resp.payload.begin() + 16);
        cout << "Registration success!" << "\n";
        // add user id to session
        session.client_id = clientID;

        // save username and client ID to "my.info"
        ofstream myInfoFile("my.info", ios::app);;
        if (myInfoFile.is_open()) {
            myInfoFile << session.username << "\n";
            myInfoFile << clientID << "\n";
            myInfoFile << session.rsaPrivateKey << "\n";
            myInfoFile << session.rsaPublicKey << "\n";
            myInfoFile.close();
            //cout << "Saved registration info to my.info with id " << session.client_id << "\n";
        }
        else {
            cerr << "Error: Could not open my.info for writing.\n";
        }
        break;
    }
    case 2101: {  //user list
        // each user record is 16 bytes for user_id + 255 bytes for username = 271 bytes.
        size_t record_size = 16 + 255; // 271 bytes per record.
        if (resp.payload.size() % record_size != 0) {
            cerr << "Error: Payload size is not a multiple of " << record_size << " bytes.\n";
            return;
        }
        size_t num_users = resp.payload.size() / record_size;
        vector<string> user_list;
        for (size_t i = 0; i < num_users; i++) {
            size_t offset = i * record_size;
            // skip the first 16 bytes (user ID), and read the next 255 bytes for username.
            string username(resp.payload.begin() + offset + 16,
                resp.payload.begin() + offset + record_size);
            // dealing null characters.
            size_t null_pos = username.find('\0');
            if (null_pos != string::npos) {
                username = username.substr(0, null_pos);
            }
            user_list.push_back(username);
        }
        // display the user list.
        cout << "for user: " << session.client_id << " display users list" << "\n";
        display_user_list(user_list); 
        break;
    }
    case 2103: {  //message sent reponse
        display_message("message sent");
        break;
    }
    case 2104: {  //pull messages response
        if (resp.payload.empty()) {
            display_message("No new messages.");
            break;
        }

        size_t offset = 0;
        while (offset < resp.payload.size()) {
            // Parse each message
            string sender_id(resp.payload.begin() + offset, resp.payload.begin() + offset + 16);
            offset += 16;
            uint32_t message_id = (resp.payload[offset] << 24) | (resp.payload[offset + 1] << 16) |
                (resp.payload[offset + 2] << 8) | resp.payload[offset + 3];
            offset += 4;
            uint8_t message_type = resp.payload[offset];
            offset += 1;
            uint32_t content_size = (resp.payload[offset] << 24) | (resp.payload[offset + 1] << 16) |
                (resp.payload[offset + 2] << 8) | resp.payload[offset + 3];
            offset += 4;
            string message_content(resp.payload.begin() + offset, resp.payload.begin() + offset + content_size);
            offset += content_size;

            string sender_name = get_username_by_id(sender_id);
            display_message("From: " + sender_name);
            display_message("Content: " + message_content);
            display_message("-----<EOM>-----");

        }
        break;
    }
    case 9000: {
        display_message("General error occurred on the server.");
        break;
    }        
    default:
        cout << "Unknown response code: " << h.code << "\n";
        break;
    }
}


//sending input and receiving data from user
void client_function(const string& server_ip, int server_port, ClientSession &session) {
    try {
        
        while (true) {

            connect_to_server(session.socket, server_ip, server_port);
            
            int usr_input = get_user_input();

            handle_request(usr_input, session);

            Response resp = read_response(session.socket);
            //cout << "response from server was read ... " << "\n";
            handle_response(session, resp);
            //cout << "response handled successfuly " << "\n";
        }
    }
    catch (const std::exception& e) {
        display_err("Client encountered an error: " + string(e.what()));
    }
}



int main() {

    
    config cfg;
    cfg.load_file("server.info");

    // get the server IP and port from the config object
    string server_ip = cfg.get_ip();
    int server_port = cfg.get_port();


    // create and launch multiple client threads
    vector<thread> client_threads;

    boost::asio::io_context io_context;  // each client gets its own io_context
    ClientSession session(io_context);  // each client gets its own socket

    client_threads.push_back(std::thread([&]() {client_function(server_ip, server_port, session);
        }));

    // wait for all client threads to finish
    for (auto& t : client_threads) {
        t.join();
    }

    

    return 0;
}
