# Harris Ransom
# Prime Generator
# 9/14/2024

# Imports
import argparse
import math

# Generate primes up to n
def sieveOfErastosthenes(n):
	# Create a list of booleans
	prime = [True for i in range(n+1)]
	
	# Iterate through numbers
	for i in range (2, int(math.sqrt(n)) + 1):
		if (prime[i]):
			for j in range(i**2, n+1, i):
				prime[j] = False

	# Output primes
	primes = []
	for p in range(2, n+1):
		if prime[p]:
			primes.append(p)
	return primes

# MAIN
if __name__ == "__main__":
	# Get input
	parser = argparse.ArgumentParser(description="Generate primes up to a given number")
	parser.add_argument("n", type=int, help="The number to generate primes up to")
	args = parser.parse_args()
	if (args.n < 2):
		print("Please enter a number greater than 1")
		exit()

	# Generate primes
	primes = sieveOfErastosthenes(args.n)
	print(primes)
	