# Harris Ransom
# Infonet Security HW2
# Linear Feedback Shift Register PRNG
# 10/2/2024

# Imports
import numpy as np

# LFSR Implementation
def LFSR(initialBits, N, table=False):
	if (len(initialBits) != 12):
		raise ValueError("Initial bits must be 12 bits long")
	
	# Initialize LFSR
	state = initialBits

	# Calculate random bits
	# Coefficients (n-i): 4, 10, 11, 12
	outputBits = []
	if (table): 
		print("State: \t \t \t \t Output:")
	for i in range(0, N):
		sum = (state[0] + state[1] + state[2] + state[8]) % 2
		state[0:11] = state[1:12]
		state[11] = sum
		outputBits.append(sum)
		if (table):
			print(f"{state} \t {sum}")
	return outputBits

# MAIN
if __name__ == "__main__":
	# Generate bits with LFSR
	initialBits = np.array([0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0])
	N = 4095
	randomBits = LFSR(initialBits, N)

	# Calculate 0 and (0, 0) frequencies
	zeroCount = 0
	zeroPairCount = 0
	for i in range(0, len(randomBits) - 1):
		if (randomBits[i] == 0):
			zeroCount += 1
			if (randomBits[i + 1] == 0):
				zeroPairCount += 1
	if (randomBits[-1] == 0):
		zeroCount += 1
	print(f"Number of 0s: {zeroCount}")
	print(f"Number of (0,0) pairs: {zeroPairCount}")
	freqZeros = zeroCount / len(randomBits)
	freqZeroPairs = zeroPairCount / (len(randomBits) - 1)
	print(f"Frequency of 0s: {freqZeros}")
	print(f"Frequency of (0,0) pairs: {freqZeroPairs}")