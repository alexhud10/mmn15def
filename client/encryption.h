#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>
#include <unordered_map>
#include "AESWrapper.h"
#include "client.h"
#include "RSAWrapper.h"

extern std::unordered_map<std::string, AESWrapper> symmetric_keys;

std::string request_public_key(const std::string& recipient_id, ClientSession& session);
void send_symmetric_key(const std::string& recipient_id, const std::string& public_key, ClientSession& session, const std::string& sender_id);
std::string encrypt_message_for_user(const std::string& recipient_id, const std::string& message);
void save_received_symmetric_key(const std::string& sender_id, const std::string& encrypted_key, RSAPrivateWrapper& rsa_priv);
bool has_symmetric_key_for_user(const std::string& user_id);

#endif // ENCRYPTION_H
