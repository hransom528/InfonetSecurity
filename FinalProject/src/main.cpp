// Infonet Security Final Project
// 12/15/2024

// Includes
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <stdlib.h>
#include <string.h>
using namespace std;

// Message Preprocessing Class Definition
class MessageBlock {
	private:
		int sizeBytes; // n but in bytes

	public:
		uint8_t n; // bits
		char *message;

		// Constructor
		MessageBlock(uint8_t n) {
			this->n = n;
			this->sizeBytes = (int) ceil(n / 8);

			// Allocate zero buffer of size sizeBytes
			this->message = (char *) calloc(sizeBytes, 1);
		}

		// Constructor with message 
		MessageBlock(uint8_t n, void *messageBuf) {
			this->n = n;
			this->sizeBytes = (int) ceil(n / 8);

			// Assign given buffer to message
			this->message = (char *) messageBuf;
		}

		// Get size in bytes
		int getSize() {
			return sizeBytes;
		}
};

// Class that defines an encoded concatenation object for message preprocessing
class EncodedBlocks {
	public:
		uint8_t k : 5;
		vector<MessageBlock> messageBlocks;

	// Constructor
	EncodedBlocks(uint8_t k) {
		// Initialize message count k
		if (k > 32) {
			throw std::invalid_argument("Size k is too large for 5 bits!");
		}
		this->k = k;
	}

	// Constructor with pre-defined message block vector
	EncodedBlocks(uint8_t k, vector<MessageBlock> vec) {
		// Initialize message count k
		if (k > 32) {
			throw std::invalid_argument("Size k is too large for 5 bits!");
		}
		this->k = k;

		// Copy message block vector to member variable
		this->messageBlocks = vec; 
	}

	// Appends existing message block to vector
	int AddMessageBlock(MessageBlock *block) {
		// Check size of message block vector
		if (messageBlocks.size() >= this->k) {
			throw std::length_error("Message block queue full for given k");
			return -1;
		}
		
		// TODO: Add message block to vector
		
		return messageBlocks.size();
	}

	// Encodes concatenated binary string into a new buffer
	const void *EncodeBuffer() {
		// Compute allocation size based on ni's
		// TODO: Implement checks on n to prevent buffer overflow possibilities
		int allocationSize = 1 + this->k;
		for (int i = 0; i < k; i++) {
			allocationSize += messageBlocks.at(i).getSize(); // Get byte size from bits
		}

		// Create buffer of allocationSize bytes
		char *buf = (char *) malloc(allocationSize);

		// Copy data into buffer
		buf[0] = this->k;
		for (int i = 0; i < k; i++) {
			MessageBlock block = messageBlocks.at(i);
			memcpy(buf, &block.n, 1);
			memcpy(buf, &block.message, block.getSize());
		}
		
		const void *returnBuf = buf;
		return returnBuf;
	}

	// TODO: Implement buffer decoding
	int DecodeBuffer(const void *buffer) {

	}
};



// MAIN
int main(int argc, char * argv[]) {
	// TODO: Randomly choose k in {0...31}

	// TODO: Randomly choose ni's

	// TODO: Randomly generate messages

	EncodedBlocks concat(3);
	return 0;
}