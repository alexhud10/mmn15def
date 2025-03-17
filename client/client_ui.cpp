#include "client_ui.h"
#include <iostream>
#include <string>
#include "network.h"  // For networking functions like send_data, receive_data

using namespace std;

// Function to get user input (message to send)
int get_user_input() {
    int option;
    cout << "\nChoose an option:" << endl;
    cout << "110 - Register User" << endl;
    cout << "120 - Request for clients list" << endl;
    cout << "150 - Send a text message" << endl;
    cout << "Enter your choice: ";
    cin >> option;
    cin.ignore();  // To discard the leftover newline character
    return option;
}

// Function to display the message received from the server
void display_message(const string& message) {
    cout << "Server response: " << message << endl;
}

// Function to display error messages
void display_err(const string& error_message) {
    cerr << "Error: " << error_message << endl;
}

void display_user_list(const vector<string>& user_list) {
    cout << "User List:" << endl;
    for (const auto& user : user_list) {
        cout << " - " << user << endl;
    }
}
