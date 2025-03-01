#include "client_ui.h"
#include <iostream>
#include <string>
#include "network.h"  // For networking functions like send_data, receive_data

using namespace std;

// Function to get user input (message to send)
string get_user_input() {
    string message;
    cout << "Enter your message: ";
    getline(cin, message);  // Get the entire message input from the user
    return message;
}

// Function to display the message received from the server
void display_message(const string& message) {
    cout << "Server response: " << message << endl;
}

// Function to display error messages
void display_err(const string& error_message) {
    cerr << "Error: " << error_message << endl;
}
