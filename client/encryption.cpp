//encryption code for messages encryption 

#include "encryption.h"
#include "RSAWrapper.h"
#include "AESWrapper.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include "client.h" 


std::unordered_map<std::string, AESWrapper> symmetric_keys;
std::unordered_map<std::string, std::string> known_public_keys;

/*


void send_symmetric_key(const std::string& recipient_id, const std::string& public_key, ClientSession& session, const std::string& sender_id) {
    AESWrapper aes;
    symmetric_keys[recipient_id] = aes;

    RSAPublicWrapper rsa_pub(public_key);
    std::string encrypted_key = rsa_pub.encrypt((const char*)aes.getKey(), AESWrapper::DEFAULT_KEYLENGTH);

    // Send request 603 with message_type=2, content=encrypted_key
    MessagePayload payload;
    memcpy(payload.recipient_id, recipient_id.c_str(), 16);
    payload.message_type = 2;
    payload.message_content = encrypted_key;
    payload.content_size = encrypted_key.size();

    std::vector<uint8_t> binary_payload = message_payload_to_binary(payload);

    Header header;
    std::string sid = sender_id;
    if (sid.size() < 16) sid.append(16 - sid.size(), '\0');
    memcpy(header.client_id, sid.data(), 16);
    header.version = 1;
    header.code = 603;
    header.payload_size = binary_payload.size();

    std::vector<uint8_t> packet = header_to_binary(header);
    packet.insert(packet.end(), binary_payload.begin(), binary_payload.end());

    send_data(session.socket, packet);
}

std::string encrypt_message_for_user(const std::string& recipient_id, const std::string& message) {
    if (symmetric_keys.find(recipient_id) == symmetric_keys.end()) {
        throw std::runtime_error("No symmetric key found for this user. Must request it first.");
    }
    AESWrapper& aes = symmetric_keys[recipient_id];
    return aes.encrypt(message.c_str(), message.size());
}

void save_received_symmetric_key(const std::string& sender_id, const std::string& encrypted_key, RSAPrivateWrapper& rsa_priv) {
    std::string sym_key = rsa_priv.decrypt(encrypted_key);
    AESWrapper aes((const unsigned char*)sym_key.c_str(), AESWrapper::DEFAULT_KEYLENGTH);
    symmetric_keys[sender_id] = aes;
}

bool has_symmetric_key_for_user(const std::string& user_id) {
    return symmetric_keys.find(user_id) != symmetric_keys.end();
}

*/
