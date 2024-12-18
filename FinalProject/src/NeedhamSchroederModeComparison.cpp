#include <iostream>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <string>
#include <unordered_map>
#include <bitset>
#include <cmath>
#include <random>
#include <sstream>
#include <stdlib.h>
#include <stdexcept>
#include <vector>
#include <string.h>
#include <algorithm> 
#include <chrono> 
#include "MessagePreprocess.h"
using namespace std;

void generate_random_key(unsigned char* key, size_t size) {
    RAND_bytes(key, size);
}

// AES encryption in CBC mode
void aes_cbc_encrypt(const unsigned char* plaintext, unsigned char* ciphertext, size_t length, const unsigned char* key, unsigned char* iv) {
    AES_KEY aesKey;
    AES_set_encrypt_key(key, 128, &aesKey);
    unsigned char temp_iv[AES_BLOCK_SIZE];
    memcpy(temp_iv, iv, AES_BLOCK_SIZE);
    AES_cbc_encrypt(plaintext, ciphertext, length, &aesKey, temp_iv, AES_ENCRYPT);
}

void aes_cbc_decrypt(const unsigned char* ciphertext, unsigned char* plaintext, size_t length, const unsigned char* key, unsigned char* iv) {
    AES_KEY aesKey;
    AES_set_decrypt_key(key, 128, &aesKey);
    unsigned char temp_iv[AES_BLOCK_SIZE];
    memcpy(temp_iv, iv, AES_BLOCK_SIZE);
    AES_cbc_encrypt(ciphertext, plaintext, length, &aesKey, temp_iv, AES_DECRYPT);
}

// AES encryption in CTR mode
void aes_ctr_encrypt(const unsigned char* plaintext, unsigned char* ciphertext, size_t length, const unsigned char* key, unsigned char* iv) {
    AES_KEY aesKey;
    AES_set_encrypt_key(key, 128, &aesKey);
    
    unsigned char counter[AES_BLOCK_SIZE];
    unsigned char keystream[AES_BLOCK_SIZE];
    memcpy(counter, iv, AES_BLOCK_SIZE);
    
    for (size_t i = 0; i < length; i += AES_BLOCK_SIZE) {
        AES_encrypt(counter, keystream, &aesKey);
        
        size_t block_size = std::min<size_t>(AES_BLOCK_SIZE, length - i); 
        for (size_t j = 0; j < block_size; ++j) {
            ciphertext[i + j] = plaintext[i + j] ^ keystream[j];
        }
        
        for (int j = AES_BLOCK_SIZE - 1; j >= 0; --j) {
            if (++counter[j] != 0) break;
        }
    }
}

// AES encryption
std::string aes_ecb_encrypt(const unsigned char* plaintext, const unsigned char* key) {
    unsigned char ciphertext[AES_BLOCK_SIZE];

    AES_KEY aesKey;
    AES_set_encrypt_key(key, 128, &aesKey);
    AES_ecb_encrypt(plaintext, ciphertext, &aesKey, AES_ENCRYPT);

    return std::string((char*)ciphertext, AES_BLOCK_SIZE);
}

// AES decryption
std::string aes_ecb_decrypt(const std::string& ciphertext, const unsigned char* key) {
    unsigned char plaintext[AES_BLOCK_SIZE];

    AES_KEY aesKey;
    AES_set_decrypt_key(key, 128, &aesKey);
    AES_ecb_encrypt(reinterpret_cast<const unsigned char*>(ciphertext.data()), plaintext, &aesKey, AES_DECRYPT);

    return std::string(reinterpret_cast<char*>(plaintext), AES_BLOCK_SIZE);
}


void aes_ctr_decrypt(const unsigned char* ciphertext, unsigned char* plaintext, size_t length, const unsigned char* key, unsigned char* iv) {
    aes_ctr_encrypt(ciphertext, plaintext, length, key, iv);
}

struct User {
    string id;
    unsigned char key[AES_BLOCK_SIZE];
};

int main() {
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

    unsigned char nonce_A[AES_BLOCK_SIZE];
    generate_random_key(nonce_A, AES_BLOCK_SIZE);

    unsigned char nonce_B[AES_BLOCK_SIZE];
    generate_random_key(nonce_B, AES_BLOCK_SIZE);

    unsigned char iv[AES_BLOCK_SIZE];
    RAND_bytes(iv, AES_BLOCK_SIZE);
    
    unsigned char plaintext[256];
    generate_random_key(plaintext, 256);
    
    unsigned char ciphertext_cbc[256];
    unsigned char ciphertext_ctr[256];
    unsigned char decrypted_cbc[256];
    unsigned char decrypted_ctr[256];
    
    // Performance measurement for CBC mode
    auto start_cbc = std::chrono::high_resolution_clock::now();
    aes_cbc_encrypt(plaintext, ciphertext_cbc, 256, Alice.key, iv);
    aes_cbc_decrypt(ciphertext_cbc, decrypted_cbc, 256, Alice.key, iv);
    auto end_cbc = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_cbc = end_cbc - start_cbc;
    std::cout << "Time taken by CBC mode: " << elapsed_cbc.count() << " seconds" << std::endl;
    
    // Performance measurement for CTR mode
    auto start_ctr = std::chrono::high_resolution_clock::now();
    aes_ctr_encrypt(plaintext, ciphertext_ctr, 256, Alice.key, iv);
    aes_ctr_decrypt(ciphertext_ctr, decrypted_ctr, 256, Alice.key, iv);
    auto end_ctr = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_ctr = end_ctr - start_ctr;
    std::cout << "Time taken by CTR mode: " << elapsed_ctr.count() << " seconds" << std::endl;
    
    // Performance measurement for ECB mode
    auto start_ecb = std::chrono::high_resolution_clock::now();
    string ciphertext = aes_ecb_encrypt(plaintext, Alice.key);
    aes_ecb_decrypt(ciphertext, Alice.key);
    auto end_ecb = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_ecb = end_ecb - start_ecb;
    std::cout << "Time taken by ECB mode: " << elapsed_ecb.count() << " seconds" << std::endl;

    return 0;
}
