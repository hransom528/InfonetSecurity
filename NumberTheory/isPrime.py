# Harris Ransom
# 10/09/2024

# Imports
from math import sqrt

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

# MAIN
if __name__ == "__main__":
    nonPrime = 144
    prime = 67
    print(f"{prime} is prime: {isPrime(prime)}")
    print(f"{nonPrime} is not prime: {isPrime(nonPrime)}")
    