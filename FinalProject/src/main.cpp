// Infonet Security Final Project
// 12/15/2024

// Includes
#include <bitset>
#include <cmath>
#include <random>
#include "main.h"
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