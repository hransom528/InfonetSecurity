#include <iostream>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <string>
#include <unordered_map>
#include "MessagePreprocess.h"

// RSA encryption
std::string rsa_encrypt(const std::string& plaintext, RSA* key) {
    std::string ciphertext(RSA_size(key), '\0');
    int len = RSA_public_encrypt(plaintext.size(), 
                                 (const unsigned char*)plaintext.c_str(), 
                                 (unsigned char*)ciphertext.data(), 
                                 key, 
                                 RSA_PKCS1_OAEP_PADDING);
    return len == -1 ? "" : ciphertext;
}

// RSA decryption
std::string rsa_decrypt(const std::string& ciphertext, RSA* key) {
    std::string plaintext(RSA_size(key), '\0');
    int len = RSA_private_decrypt(ciphertext.size(), 
                                  (const unsigned char*)ciphertext.c_str(), 
                                  (unsigned char*)plaintext.data(), 
                                  key, 
                                  RSA_PKCS1_OAEP_PADDING);
    return len == -1 ? "" : plaintext;
}

struct User {
    std::string id;
    RSA* public_key;
    RSA* private_key;
};

void generate_rsa_keypair(User& user) {
    RSA* keypair = RSA_generate_key(2048, RSA_F4, nullptr, nullptr);
    user.public_key = RSAPublicKey_dup(keypair);
    user.private_key = RSAPrivateKey_dup(keypair);
    RSA_free(keypair);
}

int main() {
    std::unordered_map<std::string, User> ttp_user_table;

    User Alice = {"Alice"};
    User Bob = {"Bob"};
    User TTP = {"TTP"};

    generate_rsa_keypair(Alice);
    generate_rsa_keypair(Bob);
    generate_rsa_keypair(TTP);

    ttp_user_table[Alice.id] = Alice;
    ttp_user_table[Bob.id] = Bob;

    // Step 1: Alice → TTP: E(KeA, IDA, IDB)
    std::string step1_message = rsa_encrypt(Alice.id + Bob.id, TTP.public_key);
    std::cout << "Step 1: Alice to TTP encrypted message: " << step1_message << std::endl;

    // Step 2: TTP → Alice: E(KdS, IDB∥KeB)
    std::string step2_response = rsa_encrypt(Bob.id + rsa_encrypt("Bob's public key", TTP.private_key), Alice.public_key);
    std::cout << "Step 2: TTP to Alice encrypted message: " << step2_response << std::endl;

    // Step 3: Alice → Bob: E(KeB, NA, IDA)
    std::string nonce_A = "random_nonce_A"; // Simplified for demonstration purposes
    std::string step3_message = rsa_encrypt(nonce_A + Alice.id, Bob.public_key);
    std::cout << "Step 3: Alice to Bob encrypted key: " << step3_message << std::endl;

    // Step 4: Bob → TTP: E(Ke,B, IDB∥IDA)
    std::string step4_message = rsa_encrypt(Bob.id + Alice.id, TTP.public_key);
    std::cout << "Step 4: Bob to TTP encrypted message: " << step4_message << std::endl;

    // Step 5: TTP → Bob: E(KdS, IDA∥KeA)
    std::string step5_response = rsa_encrypt(Alice.id + rsa_encrypt("Alice's public key", TTP.private_key), Bob.public_key);
    std::cout << "Step 5: TTP to Bob encrypted message: " << step5_response << std::endl;

    // Step 6: Bob → Alice: E(KeA, NA∥NB)
    std::string nonce_B = "random_nonce_B"; // Simplified for demonstration purposes
    std::string step6_response = rsa_encrypt(nonce_A + nonce_B, Alice.public_key);
    std::cout << "Step 6: Bob to Alice, encrypted message (NA∥NB): " << step6_response << std::endl;

    // Step 7: Alice → Bob: E(KeB, NB)
    std::string step7_response = rsa_encrypt(nonce_B, Bob.public_key);
    std::cout << "Step 7: Alice to Bob, encrypted message (NB): " << step7_response << std::endl;

    return 0;
}
