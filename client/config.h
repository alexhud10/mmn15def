#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class config {
public:
    config();  // constructor
    void load_file(const std::string& filename);  // loads file with port and ip info
    std::string get_ip() const;  
    int get_port() const;  

private:
    std::string serverIP;  
    int serverPort;  
};

#endif


