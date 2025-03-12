/*
opens file with connection info
*/

#include "config.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

//constructor with default values
config::config() : serverIP("127.0.0.1"), serverPort(1234) {}  

void config::load_file(const string& filename) {
    ifstream configFile(filename);
    if (configFile.is_open()) {
        string line;
        getline(configFile, line);  

        // get port and ip address
        size_t colonPos = line.find(":");
        if (colonPos != string::npos) {
            serverIP = line.substr(0, colonPos);  
            serverPort = stoi(line.substr(colonPos + 1));  
        }
        else {
            cerr << "Error! using default values." << std::endl;
        }
        configFile.close();
    }
    else {
        cerr << "Could not open file, using default values." << std::endl;
    }
}

// Getter for the server IP
string config::get_ip() const {
    return serverIP;
}

// Getter for the server port
int config::get_port() const {
    return serverPort;
}

