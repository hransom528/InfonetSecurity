# Harris Ransom
# Infonet Security HW3
# RSA Implementation
# 10/22/2024

# Imports
from math import gcd, sqrt
import secrets
import sympy

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

# Generate a random prime number between a lower and upper bound
# See: https://stackoverflow.com/questions/21043075/generating-large-prime-numbers-in-python
def rand_prime(bits):
    while True:
        p = secrets.randbits(bits)
        if (sympy.isprime(p)):
            break
    return p

# RSA Key Generation
def RSA_key_generate(e=65537):
    p = 0
    q = 0
    phi = 0

    # Check if totient(p*q) and e are coprime
    while (gcd(e, phi) != 1) or (e >= phi):
        # Securely pick primes p and q
        p = rand_prime(512)
        q = rand_prime(512)
        while (p == q):
            q = rand_prime(512)
        phi = (p-1)*(q-1)
        #print(f"Primes: {p}, {q}")
    
    # Output/return keys
    n = p*q
    d = inverse_finder(e, phi)
    ke = (e, n)
    kd = (d, n)
    return ke, kd

# RSA Encryption
def RSA_encrypt(m, ke):
    msg = int.from_bytes(bytes(m, 'utf-8'), 'little')
    e, n = ke[0], ke[1]
    c = exponentiation(msg, e, n)
    return c

# RSA Decryption
def RSA_decrypt(c, kd):
    d, n = kd[0], kd[1]
    m = exponentiation(c, d, n)
    msg = m.to_bytes((m.bit_length() + 7) // 8, 'little').decode('utf-8')
    return msg

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
    
    # Test RSA encryption/decryption
    e = 65537
    ke, kd = RSA_key_generate(e)
      
    message = "123456789"
    print(f"Original message: {message}")
    ciphertext = RSA_encrypt(message, ke)
    plaintext = RSA_decrypt(ciphertext, kd)
    print(f"Decrypted plaintext: {plaintext}")

    message = "hello world!"
    print(f"Original message: {message}")
    ciphertext = RSA_encrypt(message, ke)
    plaintext = RSA_decrypt(ciphertext, kd)
    print(f"Decrypted plaintext: {plaintext}")
