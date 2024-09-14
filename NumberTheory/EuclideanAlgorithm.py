# Harris Ransom
# Euclidean GCD Algorithm
# 9/14/2024

# Euclidean Algorithm
def gcd(a, b):
	if b == 0:
		return a, 
	elif a == b:
		return a
	else:
		remainder = a % b
		return gcd(b, remainder)

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

# MAIN
if __name__ == "__main__":
	while (True):
		# Get input
		a = int(input("Enter a: "))
		b = int(input("Enter b: "))

		# Calculate GCD
		d = gcd(a, b)
		print(f"The GCD of {a} and {b} is {d}\n")
