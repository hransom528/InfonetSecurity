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
#include "MessagePreprocess.h"
using namespace std;

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

// User struct
struct User {
    string id;
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

    unsigned char nonce_A[AES_BLOCK_SIZE];
    generate_random_key(nonce_A, AES_BLOCK_SIZE);

    unsigned char nonce_B[AES_BLOCK_SIZE];
    generate_random_key(nonce_B, AES_BLOCK_SIZE);
    
    // Relevant Message Blocks
    MessageBlock<unsigned char> Alice_ID(Alice.id.size(), vector<unsigned char>(Alice.id.begin(), Alice.id.end()));
    MessageBlock<unsigned char> Bob_ID(Bob.id.size(), vector<unsigned char>(Bob.id.begin(), Bob.id.end()));
    MessageBlock<unsigned char> Alice_Nonce(AES_BLOCK_SIZE, vector<unsigned char>(nonce_A, nonce_A + AES_BLOCK_SIZE));
    MessageBlock<unsigned char> Bob_Nonce(AES_BLOCK_SIZE, vector<unsigned char>(nonce_B, nonce_B + AES_BLOCK_SIZE));
    MessageBlock<unsigned char> Session_Key(AES_BLOCK_SIZE, vector<unsigned char>(session_key, session_key + AES_BLOCK_SIZE));


    // Additional steps as per the paper:
    // A - > B A
    EncodedBlocks A_to_B(1);
    A_to_B.AddMessageBlock(&Alice_ID);
    const unsigned char *encoded_first_message = A_to_B.EncodeBuffer();

    // B - > A {A,J}KB, where J is a nonce identifier which will be kept by B
    unsigned char nonce_J[AES_BLOCK_SIZE];
    generate_random_key(nonce_J, AES_BLOCK_SIZE);
    MessageBlock<unsigned char> J_Nonce(AES_BLOCK_SIZE, vector<unsigned char>(nonce_J, nonce_J + AES_BLOCK_SIZE));
    EncodedBlocks B_to_A(2);
    B_to_A.AddMessageBlock(&Alice_ID);
    B_to_A.AddMessageBlock(&J_Nonce);
    const unsigned char *encoded_nonce_message = B_to_A.EncodeBuffer();
    string nonce_message = aes_encrypt(encoded_nonce_message, Bob.key);
    
    // Step 1: Alice → TTP: E(KA, IDA∥IDB ∥NA)
    // paper protocol: A - > AS (A,B,{A,J} KB)
    // modified step 1: Alice → TTP: E(KA, IDA∥IDB ∥NA ∥E(KB, IDA || J))
    EncodedBlocks step1A_blocks(2);
    step1A_blocks.AddMessageBlock(&Alice_ID);
    step1A_blocks.AddMessageBlock(&J_Nonce);
    const unsigned char *encoded_step1A = step1A_blocks.EncodeBuffer();
    string step1A = aes_encrypt(encoded_step1A, Bob.key);
    MessageBlock<unsigned char> additional_nonce(step1A.size(), vector<unsigned char>(step1A.begin(), step1A.end()));
    
    EncodedBlocks step1_blocks(4);
    step1_blocks.AddMessageBlock(&Alice_ID);
    step1_blocks.AddMessageBlock(&Bob_ID);
    step1_blocks.AddMessageBlock(&Alice_Nonce);
    step1_blocks.AddMessageBlock(&additional_nonce);
    const unsigned char *encoded_step1 = step1_blocks.EncodeBuffer();
    cout << "Step 1: Alice to TTP unencrypted message: " << step1_blocks.toString() << endl;
    string step1_message = aes_encrypt(encoded_step1, Alice.key);

    // Step 2: TTP → Alice: E(KA, IDB ∥NA∥KAB ∥E(KB , KAB ∥IDA))
    // paper protocol: AS - > A {CK,A,J}KB where CK is session key    
    // modified step 2: TTP → Alice: E(KA, IDB ∥NA∥KAB ∥E(KB , KAB ∥IDA || J))
    EncodedBlocks step2A_blocks(3);
    step2A_blocks.AddMessageBlock(&Session_Key);
    step2A_blocks.AddMessageBlock(&Alice_ID);
    step2A_blocks.AddMessageBlock(&J_Nonce);
    const unsigned char *encoded_step2A = step2A_blocks.EncodeBuffer();
    string step2A = aes_encrypt(encoded_step2A, Bob.key);
    MessageBlock<unsigned char> KB_KAB_IDA_J(step2A.size(), vector<unsigned char>(step2A.begin(), step2A.end()));
    
    EncodedBlocks step2_blocks(4);
    step2_blocks.AddMessageBlock(&Bob_ID);
    step2_blocks.AddMessageBlock(&Alice_Nonce);
    step2_blocks.AddMessageBlock(&Session_Key);
    step2_blocks.AddMessageBlock(&KB_KAB_IDA_J);
    const unsigned char *encoded_step2 = step2_blocks.EncodeBuffer();
    cout << "Step 2: TTP to Alice unencrypted message: " << step2_blocks.toString() << endl;
    string step2_message = aes_encrypt(encoded_step2, Alice.key);

    // Step 3: Alice → Bob: E(KB , KAB ∥IDA)
    EncodedBlocks step3_blocks(2);
    step3_blocks.AddMessageBlock(&Session_Key);
    step3_blocks.AddMessageBlock(&Alice_ID);
    const unsigned char *encoded_step3 = step3_blocks.EncodeBuffer();
    cout << "Step 3: Alice to Bob unencrypted key: " << step3_blocks.toString() << endl;
    string step3_message = aes_encrypt(encoded_step3, Bob.key);

    // Step 4: Bob → Alice: E(KAB , NB) 
    EncodedBlocks step4_blocks(1);
    step4_blocks.AddMessageBlock(&Bob_Nonce);
    const unsigned char *encoded_step4 = step4_blocks.EncodeBuffer();
    cout << "Step 4: Bob to Alice unencrypted nonce: " << step4_blocks.toString() << endl;
    string step4_message = aes_encrypt(encoded_step4, session_key);

    // Step 5: Alice → Bob: E(KAB , NB −1)
    for (int i = AES_BLOCK_SIZE - 1; i >= 0; --i) {
        if (nonce_B[i] > 0) {
            nonce_B[i]--;
            break;
        }
    }
    string step5_message = aes_encrypt(nonce_B, session_key);
    cout << "Step 5: Alice to Bob, encrypted (NB-1): " << step5_message << endl;

    cout << "\nIdentical Block Test" << endl;
    EncodedBlocks block_test_1(1);
    block_test_1.AddMessageBlock(&Alice_ID);
    const unsigned char *encoded_block_test_1 = block_test_1.EncodeBuffer();
    string block_test_1_message = aes_encrypt(encoded_block_test_1, session_key);

    EncodedBlocks block_test_2(1);
    block_test_2.AddMessageBlock(&Alice_ID);
    const unsigned char *encoded_block_test_2 = block_test_2.EncodeBuffer();
    string block_test_2_message = aes_encrypt(encoded_block_test_2, session_key);

    cout << "Encoded Block Test 1: " << block_test_1_message << endl;
    cout << "Encoded Block Test 2: " << block_test_2_message << endl;
    cout << "Identical ciphertext? " << bool(block_test_1_message.compare(block_test_2_message) == 0) << endl;

    return 0;
}