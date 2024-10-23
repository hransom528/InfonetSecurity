# Harris Ransom
# Infonet Security HW3
# RSA Implementation
# 10/22/2024

# Imports
from math import gcd, sqrt
import secrets

# Helper function to check if a number is prime
def isPrime(n):
    prime_flag = 0

    if(n > 1):
        for i in range(2, int(sqrt(n)) + 1):
            if (n % i == 0):
                prime_flag = 1
                break
        if (prime_flag == 0):
            return True
        else:
            return False
    else:
        return False

# Euler Totient Function
def totient(n):
	count = 0
	for i in range (1, n+1):
		if (gcd(n, i) == 1):
			count += 1
	return count

# Extended Euclidean Algorithm
def gcdExtended(a, b):
	# Base Case
	if a == 0:
		return b, 0, 1
	
	gcd, x1, y1 = gcdExtended(b % a, a)
		
	# Update x and y using results of recursive call
	x = y1 - (b//a) * x1
	y = x1
	return gcd, x, y

# Auxiliary Functions
def exponentiation(m, e, n):
	# return (m ** e) % n

	# Implements efficient exponentiation algorithm
	r = 1 # Initialize result to 1
	e_bin = bin(e)[2:]
	for i in e_bin:
		r = (r**2) % n
		if (i == '1'):
			r = (r*m) % n
	return r
def inverse_finder(a, n):
	isCoprime = (gcd(a,n) == 1)
	if (not isCoprime):
		raise ValueError("a and n are not coprime!")

	# TODO: Implement modular inverse finder
	_, x, y = gcdExtended(a, n)
	result = (x % n + n) % n
	return result

# RSA Key Generation
def RSA_key_generate(e=65537):
	p = 0
	q = 0

	# Check if totient(p*q) and e are coprime
	while (not gcd(totient(p*q), e) == 1) or (e < totient(p*q)):
		# TODO: Fix prime generation

		# Securely pick prime p
		p = secrets.randbelow(2**512)
		while (not isPrime(p) or (p < e)):
			p = secrets.randbelow(2**512)
		
		# Securely pick prime q
		q = secrets.randbelow(2**512)
		while (not isPrime(q) or (q < e)):
			q = secrets.randbelow(2**512)
    
	# Output/return keys
	n = p*q
	d = inverse_finder(e, totient(n))
	ke = (e, n)
	kd = (d, n)
	return ke, kd

# RSA Encryption
def RSA_encrypt(m, e, n):
	pass

# RSA Decryption
def RSA_decrypt(c, d, n):
	pass

# MAIN
if __name__ == "__main__":
	# Test exponentiation
	expResult = exponentiation(2000, 2020, 2023)
	print(f"Exponentiation result: {expResult}")

	# Test inverse_finder
	invResult = inverse_finder(2000, 2023)
	print(f"Modular Inverse Finder result: {invResult}")

	# Test RSA keygen
	print("RSA Keygen Tests:")
	for i in range(10):
		ke, kd = RSA_key_generate(e=3)
		print(f"{i}.) {ke} \t {kd}")
	
	# TODO: Test RSA encryption/decryption