// helper functions

#include "utils.h"
#include <iostream>
#include <string>
#include <fstream>
#include "client_ui.h" 
#include <filesystem>


// get id from my.info
string get_id_by_username(const string& username) {

    ifstream file("my.info");

    if (!file.is_open()) {
        display_err("Error: Could not open my.info for reading");
        return "";
    }

    string file_username, file_userid, public_key;
    while (getline(file, file_username) && getline(file, file_userid) && getline(file, public_key)) {
        if (file_username == username) {
            return file_userid;
        }
    }

    display_err("Username not found in my.info");
    return "";
}

//check if user name is in my.info
bool user_in_file(const string& username) {
    ifstream file("my.info");
    if (!file.is_open()) {
        return false;  
    }

    string file_username, file_userid;
    while (getline(file, file_username) && getline(file, file_userid)) {
        if (file_username == username) {
            return true;  
        }
    }

    return false;  
}

string get_username_by_id(const string& user_id) {
    ifstream file("my.info");
    if (!file.is_open()) {
        display_err("Error: Could not open my.info for reading");
        return user_id;  // fallback: return the ID
    }

    string file_username, file_userid;
    while (std::getline(file, file_username) && getline(file, file_userid)) {
        if (file_userid == user_id) {
            return file_username;
        }
    }

    return user_id;  // fallback if not found
}