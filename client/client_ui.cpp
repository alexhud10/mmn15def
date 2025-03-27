#include "client_ui.h"
#include <iostream>
#include <string>
#include "network.h"  


using namespace std;

// function to get user input (message to send)
// input cant be string or number that does not exist
int get_user_input() {
    string input;
    cout << "\nChoose an option:" << endl;
    cout << "110 - Register User" << endl;
    cout << "120 - Request for clients list" << endl;
    cout << "130 - Request for public key" << endl;
    cout << "140 - Request for waiting messages" << endl;
    cout << "150 - Send a text message" << endl;
    cout << "0 - Exit client" << endl;
    cout << "Enter your choice: ";
    getline(cin, input); 

    try {
        int option = stoi(input);  
        return option;
    }
    catch (const exception&) {
        return -1;  
    };
}

void display_message(const string& message) {
    cout << " " << message << endl;
}

// display error messages
void display_err(const string& error_message) {
    cerr << "Error: " << error_message << endl;
}

void display_user_list(const vector<string>& user_list) {
    cout << "User List:" << endl;
    for (const auto& user : user_list) {
        cout << " - " << user << endl;
    }
}
