# Harris Ransom
# Check if list of numbers are all prime
# 9/14/2024

# Imports
import argparse
from generatePrimes import sieveOfErastosthenes

# MAIN
if __name__ == "__main__":
	# Get input
	#parser = argparse.ArgumentParser(description="Check if list of numbers are all prime")
	#parser.add_argument("nums", type=list, help="List of numbers to check")
	#args = parser.parse_args()
	nums = []
	n = int(input("Input length of list to check: "))
	for i in range(0, n):
		nums.append(int(input()))
	
	# Check list against Sieve of Erastosthenes
	maxNum = max(nums)
	primes = sieveOfErastosthenes(maxNum)
	if set(nums).issubset(primes):
		print('Nums are all primes')
	else:
		print('Nums are not all primes')