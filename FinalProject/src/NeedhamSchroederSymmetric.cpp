#include <iostream>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <string>
#include <unordered_map>

void generate_random_key(unsigned char* key, size_t size) {
    RAND_bytes(key, size);
}

// AES encryption
std::string aes_encrypt(const unsigned char* plaintext, const unsigned char* key) {
    unsigned char ciphertext[AES_BLOCK_SIZE];

    AES_KEY aesKey;
    AES_set_encrypt_key(key, 128, &aesKey);
    AES_ecb_encrypt(plaintext, ciphertext, &aesKey, AES_ENCRYPT);

    return std::string((char*)ciphertext, AES_BLOCK_SIZE);
}

// AES decryption
std::string aes_decrypt(const unsigned char* ciphertext, const unsigned char* key) {
    unsigned char plaintext[AES_BLOCK_SIZE];

    AES_KEY aesKey;
    AES_set_decrypt_key(key, 128, &aesKey);
    AES_ecb_encrypt(ciphertext, plaintext, &aesKey, AES_DECRYPT);

    return std::string((char*)plaintext, AES_BLOCK_SIZE);
}

struct User {
    std::string id;
    unsigned char key[AES_BLOCK_SIZE];
};

int main() {
    // TTP table
    std::unordered_map<std::string, User> ttp_user_table;

    User Alice = {"Alice", {0}};
    User Bob = {"Bob", {0}};
    User TTP = {"TTP", {0}};

    generate_random_key(Alice.key, AES_BLOCK_SIZE);
    generate_random_key(Bob.key, AES_BLOCK_SIZE);
    generate_random_key(TTP.key, AES_BLOCK_SIZE);

    ttp_user_table[Alice.id] = Alice;
    ttp_user_table[Bob.id] = Bob;

    unsigned char session_key[AES_BLOCK_SIZE];
    generate_random_key(session_key, AES_BLOCK_SIZE);

    // Step 1: Alice → TTP: E(KA, IDA∥IDB ∥NA)
    unsigned char nonce_A[AES_BLOCK_SIZE];
    generate_random_key(nonce_A, AES_BLOCK_SIZE);
    std::string step1_message = aes_encrypt((unsigned char*)(Alice.id + Bob.id + std::string((char*)nonce_A, AES_BLOCK_SIZE)).c_str(), Alice.key);
    std::cout << "Step 1: Alice to TTP encrypted message: " << step1_message << std::endl;

    // Step 2: TTP → Alice: E(KA, IDB ∥NA∥KAB ∥E(KB , KAB ∥IDA))
    std::string kb_kab_ida = aes_encrypt((unsigned char*)(std::string((char*)session_key, AES_BLOCK_SIZE) + Alice.id).c_str(), Bob.key);
    std::string step2_response = aes_encrypt((unsigned char*)(Bob.id + std::string((char*)nonce_A, AES_BLOCK_SIZE) + std::string((char*)session_key, AES_BLOCK_SIZE) + kb_kab_ida).c_str(), Alice.key);
    std::cout << "Step 2: TTP to Alice encrypted message: " << step2_response << std::endl;

    // Step 3: Alice → Bob: E(KB , KAB ∥IDA)
    std::string step3_message = aes_encrypt((unsigned char*)(std::string((char*)session_key, AES_BLOCK_SIZE) + Alice.id).c_str(), Bob.key);
    std::cout << "Step 3: Alice to Bob encrypted key: " << step3_message << std::endl;

    // Step 4: Bob → Alice: E(KAB , NB) 
    // protocol in paper 1: B -> A: {B, Na, Nb}Ka
    // adjusted Step 4: Bob → Alice: E(KAB , B || NA || NB) 
    unsigned char nonce_B[AES_BLOCK_SIZE];
    generate_random_key(nonce_B, AES_BLOCK_SIZE);
    std::string encrypted_nonce_B = aes_encrypt((unsigned char*)(Bob.id + std::string((char*)nonce_A, AES_BLOCK_SIZE) + std::string((char*)nonce_B, AES_BLOCK_SIZE)).c_str(), session_key);    
    std::cout << "Step 4: Bob to Alice, encrypted nonce (B, NA, NB): " << encrypted_nonce_B << std::endl;

    // Step 5: Alice → Bob: E(KAB , NB −1)
    for (int i = AES_BLOCK_SIZE - 1; i >= 0; --i) {
        if (nonce_B[i] > 0) {
            nonce_B[i]--;
            break;
        }
    }
    std::string step5_response = aes_encrypt(nonce_B, session_key);
    std::cout << "Step 5: Alice to Bob, encrypted (NB-1): " << step5_response << std::endl;

    return 0;
}