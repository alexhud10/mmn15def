#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class config {
public:
    config();  // constructor
    void loadFile(const std::string& filename);  // loads file with port and ip info
    std::string getIP() const;  
    int getPort() const;  

private:
    std::string serverIP;  
    int serverPort;  
};

#endif


