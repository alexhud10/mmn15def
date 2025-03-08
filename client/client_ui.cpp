#include "client_ui.h"
#include <iostream>
#include <string>
#include "network.h"  // For networking functions like send_data, receive_data

using namespace std;

// Function to get user input (message to send)
int get_user_input() {
    int option;
    cout << "Choose an option:" << endl;
    cout << "110 - Register User" << endl;
    cout << "150 - Send Message" << endl;
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
