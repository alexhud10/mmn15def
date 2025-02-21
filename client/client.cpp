#include "client.h"
#include <iostream>
#include <string>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>

using namespace CryptoPP;

int main() {
    // Create a random key and IV.
    AutoSeededRandomPool prng;
    byte key[AES::DEFAULT_KEYLENGTH];
    byte iv[AES::BLOCKSIZE];
    prng.GenerateBlock(key, sizeof(key));
    prng.GenerateBlock(iv, sizeof(iv));

    std::string plaintext = "Hello, Crypto++!";
    std::string ciphertext, recovered;

    // Encrypt the plaintext.
    try {
        CBC_Mode< AES >::Encryption encryptor;
        encryptor.SetKeyWithIV(key, sizeof(key), iv);

        StringSource ss1(plaintext, true,
            new StreamTransformationFilter(encryptor,
                new StringSink(ciphertext)
            )
        );
    }
    catch (const Exception& e) {
        std::cerr << "Encryption error: " << e.what() << std::endl;
        return 1;
    }

    // Decrypt the ciphertext.
    try {
        CBC_Mode< AES >::Decryption decryptor;
        decryptor.SetKeyWithIV(key, sizeof(key), iv);

        StringSource ss2(ciphertext, true,
            new StreamTransformationFilter(decryptor,
                new StringSink(recovered)
            )
        );
    }
    catch (const Exception& e) {
        std::cerr << "Decryption error: " << e.what() << std::endl;
        return 1;
    }

    // Output results.
    std::cout << "Plaintext: " << plaintext << std::endl;

    // Display ciphertext in hex.
    std::string encoded;
    StringSource ss3(ciphertext, true,
        new HexEncoder(
            new StringSink(encoded)
        )
    );
    std::cout << "Ciphertext (hex): " << encoded << std::endl;
    std::cout << "Recovered: " << recovered << std::endl;

    return 0;
}
