#ifndef CLIENT_H
#define CLIENT_H
#pragma once
#include <boost/asio.hpp>  
#include <string>
#include "config.h"
#include "network.h"  
#include "RSAWrapper.h"
using boost::asio::ip::tcp;
using namespace std;

class ClientSession {
public:
    std::string username; // holds the username entered by the user.
    std::string client_id; // holds id the assigned from registration
    tcp::socket socket;
    RSAPrivateWrapper rsaPrivate;
    std::string rsaPublicKey;
    std::string rsaPrivateKey;

    // constructor: initializes the socket with the io_context.
    ClientSession(boost::asio::io_context& io_context)
        : socket(io_context) {}
};

std::string get_id_by_username(const std::string& username);

void handle_request(int option, ClientSession& session);

void handle_response(ClientSession& session, const Response& resp);

void client_function(const string& server_ip, int server_port, ClientSession& session);

#endif  // CLIENT_H
