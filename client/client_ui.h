#pragma once

#ifndef CLIENT_UI_H
#define CLIENT_UI_H

#include <string>

using namespace std;

//get users input
string get_user_input();

//display messages receved from server
void display_message(const string& message);

//display errors if accurs while getting input or dispaly msg
void display_err(const string& error_message);

#endif
