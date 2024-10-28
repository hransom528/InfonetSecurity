# Harris Ransom
# Infonet Security HW3
# Pohlig-Hellman Implementation
# 10/22/2024

# Check if number is a primitive root of a modulus
def isPrimitiveRoot(g, p):
	result = g**(p-1) % p
	return (result == 1)
