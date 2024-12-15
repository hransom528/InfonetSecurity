#include <iostream>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/base64.h>
#include <cryptopp/files.h>
#include <cryptopp/cryptlib.h>
#include <unordered_map>
#include <bitset>
#include <cmath>
#include <random>
#include <sstream>
#include <stdlib.h>
#include <stdexcept>
#include <vector>
#include <string.h>
#include "MessagePreprocess.h"

const int BLOCK_SIZE = 255; 
const int MAX_RSA_CHUNK_SIZE = 214; // Max message size for 2048-bit key with OAEP

// RSA encryption with chunking
std::string rsa_encrypt_chunked(const unsigned char* plaintext, int plaintext_len, CryptoPP::RSA::PublicKey& key) {
    CryptoPP::AutoSeededRandomPool rng;
    std::string ciphertext;
    
    for (int i = 0; i < plaintext_len; i += MAX_RSA_CHUNK_SIZE) {
        int chunk_size = std::min(MAX_RSA_CHUNK_SIZE, plaintext_len - i);
        std::string encrypted_chunk;
        try {
            CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(key);
            CryptoPP::StringSource ss(plaintext + i, chunk_size, true,
                new CryptoPP::PK_EncryptorFilter(rng, encryptor, new CryptoPP::StringSink(encrypted_chunk))
            );
        } catch (const CryptoPP::Exception& e) {
            std::cerr << "Encryption failed: " << e.what() << std::endl;
            return "";
        }
        ciphertext += encrypted_chunk;
    }
    return ciphertext;
}

// RSA decryption with chunking
std::string rsa_decrypt_chunked(const std::string& ciphertext, CryptoPP::RSA::PrivateKey& key) {
    CryptoPP::AutoSeededRandomPool rng;
    std::string plaintext;
    
    size_t chunk_size = CryptoPP::RSAES_OAEP_SHA_Decryptor(key).FixedCiphertextLength();
    for (size_t i = 0; i < ciphertext.size(); i += chunk_size) {
        std::string decrypted_chunk;
        try {
            CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(key);
            CryptoPP::StringSource ss(ciphertext.substr(i, chunk_size), true,
                new CryptoPP::PK_DecryptorFilter(rng, decryptor, new CryptoPP::StringSink(decrypted_chunk))
            );
        } catch (const CryptoPP::Exception& e) {
            std::cerr << "Decryption failed: " << e.what() << std::endl;
            return "";
        }
        plaintext += decrypted_chunk;
    }
    return plaintext;
}

// Convert RSA public key to string
std::vector<MessageBlock<unsigned char> > split_rsa_public_key(CryptoPP::RSA::PublicKey& key) {
    std::string key_string;
    CryptoPP::StringSink ss(key_string);
    key.Save(ss);
    
    std::vector<MessageBlock<unsigned char> > key_blocks;
    size_t offset = 0;
    size_t total_size = key_string.size();
    
    while (offset < total_size) {
        size_t chunk_size = std::min<size_t>(BLOCK_SIZE, total_size - offset);
        std::vector<unsigned char> chunk(key_string.begin() + offset, key_string.begin() + offset + chunk_size);
        key_blocks.emplace_back(chunk_size, chunk);
        offset += chunk_size;
    }
    
    return key_blocks;
}

CryptoPP::RSA::PublicKey reassemble_rsa_public_key(const std::vector<MessageBlock<unsigned char> >& key_blocks) {
    std::string key_string;
    for (const auto& block : key_blocks) {
        key_string.append(block.message.begin(), block.message.end());
    }
    
    CryptoPP::RSA::PublicKey key;
    CryptoPP::StringSource ss(key_string, true);
    key.Load(ss);
    
    return key;
}

struct User {
    std::string id;
    CryptoPP::RSA::PublicKey public_key;
    CryptoPP::RSA::PrivateKey private_key;
};

void generate_rsa_keypair(User& user) {
    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng, 2048);
    
    user.private_key = CryptoPP::RSA::PrivateKey(params);
    user.public_key = CryptoPP::RSA::PublicKey(params);
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

    unsigned char nonce_A[BLOCK_SIZE];
    unsigned char nonce_B[BLOCK_SIZE];
    
    CryptoPP::AutoSeededRandomPool rng;
    rng.GenerateBlock(nonce_A, BLOCK_SIZE);
    rng.GenerateBlock(nonce_B, BLOCK_SIZE);

    MessageBlock<unsigned char> Alice_ID(Alice.id.size(), std::vector<unsigned char>(Alice.id.begin(), Alice.id.end()));
    MessageBlock<unsigned char> Bob_ID(Bob.id.size(), std::vector<unsigned char>(Bob.id.begin(), Bob.id.end()));
    MessageBlock<unsigned char> Alice_Nonce(BLOCK_SIZE, std::vector<unsigned char>(nonce_A, nonce_A + BLOCK_SIZE));
    MessageBlock<unsigned char> Bob_Nonce(BLOCK_SIZE, std::vector<unsigned char>(nonce_B, nonce_B + BLOCK_SIZE));
    
    std::vector<MessageBlock<unsigned char> > Bob_PublicKey_Blocks = split_rsa_public_key(Bob.public_key);
    std::vector<MessageBlock<unsigned char> > Alice_PublicKey_Blocks = split_rsa_public_key(Alice.public_key);

    // Step 1: Alice → TTP: E(KeA, IDA, IDB)
    EncodedBlocks step1_blocks(2);
    step1_blocks.AddMessageBlock(&Alice_ID);
    step1_blocks.AddMessageBlock(&Bob_ID);
    const unsigned char* encoded_step1 = step1_blocks.EncodeBuffer();
    int step1_size = step1_blocks.EncodedSize();
    std::string step1_message = rsa_encrypt_chunked(encoded_step1, step1_size, Alice.public_key);
    std::cout << "Step 1: Alice to TTP encrypted message: " << step1_message << std::endl;

    // Step 2: TTP → Alice: E(KdS, IDB∥KeB)
    EncodedBlocks step2_blocks(1 + Bob_PublicKey_Blocks.size());
    step2_blocks.AddMessageBlock(&Bob_ID);
    for (auto& block : Bob_PublicKey_Blocks) {
        step2_blocks.AddMessageBlock(&block);
    }
    const unsigned char* encoded_step2 = step2_blocks.EncodeBuffer();
    int step2_size = step2_blocks.EncodedSize();
    std::string step2_message = rsa_encrypt_chunked(encoded_step2, step2_size, TTP.private_key);
    std::cout << "Step 2: TTP to Alice encrypted message: " << step2_message << std::endl;

    // Step 3: Alice → Bob: E(KeB, NA, IDA)
    EncodedBlocks step3_blocks(2);
    step3_blocks.AddMessageBlock(&Alice_Nonce);
    step3_blocks.AddMessageBlock(&Alice_ID);
    const unsigned char* encoded_step3 = step3_blocks.EncodeBuffer();
    int step3_size = step3_blocks.EncodedSize();
    std::string step3_message = rsa_encrypt_chunked(encoded_step3, step3_size, Bob.public_key);
    std::cout << "Step 3: Alice to Bob encrypted key: " << step3_message << std::endl;

    // Step 4: Bob → TTP: E(KeB, IDB∥IDA)
    EncodedBlocks step4_blocks(2);
    step4_blocks.AddMessageBlock(&Bob_ID);
    step4_blocks.AddMessageBlock(&Alice_ID);
    const unsigned char* encoded_step4 = step4_blocks.EncodeBuffer();
    int step4_size = step4_blocks.EncodedSize();
    std::string step4_message = rsa_encrypt_chunked(encoded_step4, step4_size, Bob.public_key);
    std::cout << "Step 4: Bob to TTP encrypted message: " << step4_message << std::endl;

    // Step 5: TTP → Bob: E(KdS, IDA∥KeA)
    EncodedBlocks step5_blocks(1 + Alice_PublicKey_Blocks.size());
    step5_blocks.AddMessageBlock(&Alice_ID);
    for (auto& block : Alice_PublicKey_Blocks) {
        step5_blocks.AddMessageBlock(&block);
    }
    const unsigned char* encoded_step5 = step5_blocks.EncodeBuffer();
    int step5_size = step5_blocks.EncodedSize();
    std::string step5_message = rsa_encrypt_chunked(encoded_step5, step5_size, TTP.private_key);
    std::cout << "Step 5: TTP to Bob encrypted message: " << step5_message << std::endl;

    // Step 6: Bob → Alice: E(KeA, NA∥NB)
    // Paper 1 protocol: B -> A: {B, Na, Nb}Ka
    // Adjusted Step 6:  Bob → Alice: E(KeA, IDB || NA∥NB)
    EncodedBlocks step6_blocks(3);
    step6_blocks.AddMessageBlock(&Bob_ID);
    step6_blocks.AddMessageBlock(&Alice_Nonce);
    step6_blocks.AddMessageBlock(&Bob_Nonce);
    const unsigned char* encoded_step6 = step6_blocks.EncodeBuffer();
    int step6_size = step6_blocks.EncodedSize();
    std::string step6_message = rsa_encrypt_chunked(encoded_step6, step6_size, Alice.public_key);
    std::cout << "Step 6: Bob to Alice, encrypted message (NA∥NB): " << step6_message << std::endl;

    // Step 7: Alice → Bob: E(KeB, NB)
    EncodedBlocks step7_blocks(1);
    step7_blocks.AddMessageBlock(&Bob_Nonce);
    const unsigned char* encoded_step7 = step7_blocks.EncodeBuffer();
    int step7_size = step7_blocks.EncodedSize();
    std::string step7_message = rsa_encrypt_chunked(encoded_step7, step7_size, Bob.public_key);
    std::cout << "Step 7: Alice to Bob, encrypted message (NB): " << step7_message << std::endl;

    return 0;
}
