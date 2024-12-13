// Infonet Security Final Project
// 12/15/2024

// Includes
#include <iostream>
#include <bitset>
#include <cmath>
#include <random>
#include <sstream>
#include <stdlib.h>
#include <stdexcept>
#include <vector>
#include <string.h>
using namespace std;

// Helper function to generate a random set of bits
// See: https://stackoverflow.com/questions/25176423/c-efficient-way-to-generate-random-bitset-with-configurable-mean-1s-to-0s
vector<unsigned char> random_bitset(size_t size, double p = 0.5) {
    vector<unsigned char> bits(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d( p);

    for(size_t n = 0; n < size; ++n) {
        bits[n] = d(gen);
    }

    return bits;
}

// Helper function to print a vector
template <typename T> void printVec(vector<T> vec) {
	stringstream ss;
	for (auto i: vec) {
		cout << +i << " ";
	}
	cout << endl;
}

// Message Block Preprocessing Class Definition
template <typename T> class MessageBlock {
	public:
		uint8_t n; // bits
		vector<T> message;

		// Constructor
		MessageBlock(uint8_t n) {
			this->n = n;

			// Allocate zero buffer of size sizeBytes
			std::vector<T> message(n);
			this->message = message;
		}

		// Constructor with given message 
		MessageBlock(uint8_t n, vector<T> messageBuf) {
			this->n = n;

			// Implement checks on n to prevent buffer overflows
			if (n > 255) {
				throw std::invalid_argument("Size n is too large!");
			}
			if (n != messageBuf.size()) {
				throw std::invalid_argument("Size n does not match messageBuf!");
			}

			// Assign given buffer to message
			this->message.assign(messageBuf.begin(), messageBuf.end());
		}

		// Get size in bytes
		int getSize() {
			return message.size();
		}

		// Get string representation of MessageBlock
		string toString() {
			stringstream ss;
			ss << "[size=" << unsigned(n) << ", ";
			for (int i = 0; i < this->n; i++) {
				ss << unsigned(this->message.at(i));
				if (i != (this->n - 1))
					ss << ",";
			}
			ss << "]" << endl;
			return ss.str();
		}
};

// Class that defines an encoded block concatenation object for message preprocessing
class EncodedBlocks {
	public:
		uint8_t k;
		vector<MessageBlock<unsigned char>> messageBlocks;

	// Constructor
	EncodedBlocks(uint8_t k) {
		// Initialize message count k
		if (k > 32) {
			throw std::invalid_argument("Size k is too large for 5 bits!");
		}
		this->k = k;
	}

	// Constructor with pre-defined message block vector
	EncodedBlocks(uint8_t k, vector<MessageBlock<unsigned char>> vec) {
		// Initialize message count k
		if (k > 32) {
			throw std::invalid_argument("Size k is too large for 5 bits!");
		}
		this->k = k;

		// Copy message block vector to member variable
		this->messageBlocks.assign(vec.begin(), vec.end()); 
	}

	// Appends existing message block to vector
	int AddMessageBlock(MessageBlock<unsigned char> *block) {
		// Check size of message block vector
		if (messageBlocks.size() >= this->k) {
			throw std::length_error("Message block queue full for given k");
			return -1;
		}
		
		// Add message block to vector
		messageBlocks.push_back(*block);
		return messageBlocks.size();
	}

	// Encodes concatenated binary string into a new buffer
	const unsigned char *EncodeBuffer() {
		// Compute allocation size based on ni's
		int allocationSize = 1 + this->k;
		for (int i = 0; i < k; i++) {
			allocationSize += messageBlocks.at(i).getSize(); // Get byte size from bits
		}

		// Create buffer of allocationSize bytes
		unsigned char *buf = (unsigned char *) malloc(allocationSize);

		// Copy data into buffer
		unsigned char *currPointer = &buf[1]; // Keeps track of location in buffer through iterations
		memcpy(&buf[0], &(this->k), sizeof(uint8_t));

		for (int i = 0; i < k; i++) {
			MessageBlock<unsigned char> block = messageBlocks.at(i);
			unsigned int messageSize = block.getSize();

			*currPointer = (char) block.n;
			currPointer++;
			copy(block.message.begin(), block.message.end(), currPointer);
			currPointer += messageSize;
		}
		
		// Return concatenated buffer
		const unsigned char *returnBuf = buf;
		return returnBuf;
	}

	// Buffer decoding
	static EncodedBlocks DecodeBuffer(unsigned char *buffer) {
		// Cast buffer into usable form and decode k from start
		unsigned char *concatBuf = buffer;
		uint8_t k = (uint8_t) concatBuf[0];
		//cout << "K: " << unsigned(k) << endl;

		// Decode each message block
		vector<MessageBlock<unsigned char>> messageBlocks;
		unsigned char *currPointer = &concatBuf[1]; 
		for (int i = 0; i < k; i++) {
			// Get size	
			uint8_t ni = (uint8_t) *currPointer;
			currPointer++;

			// Get message
			vector<unsigned char> tempBuffer(currPointer, currPointer + ni);
			currPointer += ni;
			
			// Create message block and append to vector
			MessageBlock<unsigned char> block(ni, tempBuffer);
			messageBlocks.push_back(block);
		}

		// Create EncodedBlocks object
		EncodedBlocks concat(k, messageBlocks);
		return concat;
	}

	// Gets string representation of encoded message blocks
	string toString() {
		stringstream ss;

		ss << "[k=" << unsigned(this->k) << ", \n";
		for (int i = 0; i < this->k; i++) {
			ss << this->messageBlocks[i].toString();
		}
		ss << "]" << endl;
		return ss.str();
	}
};


// MAIN
int main(int argc, char * argv[]) {
	// Initialize random number generator with current time as seed
	srand(time(0));

	// Randomly choose k in {0...31}
	uint8_t k = rand() % 32;
	cout << "Chosen k: " << unsigned(k) << endl;

	// Randomly choose ni's for each i in k
	vector<uint8_t> messageSizes;
	for (int i = 0; i < k; i++) {
		uint8_t ni = (rand() % 4) + 1;
		messageSizes.push_back(ni);
	}
	cout << "Message sizes: ";
	printVec<uint8_t>(messageSizes);

	// Randomly generate messages for each ni
	vector<vector<unsigned char>> messages;
	for (int i = 0; i < k; i++) {
		uint8_t ni = messageSizes.at(i);
		vector<unsigned char> mi = random_bitset(ni, 0.5); 
		//printVec<unsigned char>(mi);
		messages.push_back(mi);
	}

	// Encode messages into message block objects
	vector<MessageBlock<unsigned char>> messageBlocks;
	for (int i = 0; i < k; i++) {
		MessageBlock<unsigned char> mb(messageSizes[i], messages[i]);
		messageBlocks.push_back(mb);
		//cout << mb.toString();
	}

	// Create encoded block object
	EncodedBlocks concat(k, messageBlocks);
	cout << concat.toString();

	// Test encoding/decoding
	unsigned char *buf = (unsigned char *) concat.EncodeBuffer();
	EncodedBlocks decodeTest = EncodedBlocks::DecodeBuffer(buf);
	cout << decodeTest.toString();
	return 0;
}