# Harris Ransom
# Infonet Security HW2
# Blum-Blum-Shub PRNG
# 10/2/2024

# Imports
from datetime import datetime
from math import gcd
import random

# Helper function to check if two numbers are relatively prime
def coprime(a, b):
    return gcd(a, b) == 1

# Generates N pseudo-random bits
def bbsGenerate(p, q, seed, N):
	prngBits = []
	x = []

	# Initialize with seed value
	x0 = (seed**2) % n
	x.append(x0)
	prngBits.append(x0 % 2)

	# Generate rest of the bits
	for i in range(0, N-1):
		xi = (x[i] ** 2) % n
		x.append(xi)
		prngBits.append(xi % 2)
	return prngBits

# MAIN
if __name__ == "__main__":
	# Choose p, q, seed
	# p = q = 3 (mod 4)
	p = (random.randint(100000000, 10000000000) * 4) + 3 
	q = (random.randint(100000000, 10000000000) * 4) + 3
	n = p * q
	seed = int(((datetime.now() - datetime(1970, 1, 1)).total_seconds()) * (10**6)) # Unix timestamp
	while (not coprime(n, seed)):
		seed = int(((datetime.now() - datetime(1970, 1, 1)).total_seconds()) * (10**6)) # Unix timestamp
	
	# Generate N pseudo-random bits
	prngBits = bbsGenerate(p, q, seed, 10**6)
	#print(prngBits)

	# Check generated bits for 0s and (0,0) pairs
	zeroCount = 0
	zeroPairCount = 0
	for i in range(0, len(prngBits) - 1):
		if (prngBits[i] == 0):
			zeroCount += 1
			if (prngBits[i + 1] == 0):
				zeroPairCount += 1
	if (prngBits[-1] == 0):
		zeroCount += 1
	print(f"Number of 0s: {zeroCount}")
	print(f"Number of (0,0) pairs: {zeroPairCount}")
	freqZeros = zeroCount / len(prngBits)
	freqZeroPairs = zeroPairCount / (len(prngBits) - 1)
	print(f"Frequency of 0s: {freqZeros}")
	print(f"Frequency of (0,0) pairs: {freqZeroPairs}")
