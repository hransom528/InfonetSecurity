# Harris Ransom
# Euclidean GCD Algorithm
# 9/14/2024

# Euclidean Algorithm (non-recursive)
def gcd(a, b):
	if (a < b):
		a,b = b,a

	iterations = 1
	while((a%b) > 0):
		remainder = a % b
		a = b
		b = remainder
		iterations += 1
	return b, iterations

# Euclidean Algorithm (recursive)
def gcdRecursive(a, b):
	if (a < b):
		a,b = b,a
	
	if b == 0:
		return a
	elif a == b:
		return a
	else:
		remainder = a % b
		return gcdRecursive(b, remainder)

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

# Least common multiple
def lcm(a, b):
	return (a*b) / (gcdRecursive(a,b))

# MAIN
if __name__ == "__main__":
	while (True):
		# Get input
		a = int(input("Enter a: "))
		b = int(input("Enter b: "))

		# Calculate GCD
		d, iterations = gcd(a, b)
		print(f"The GCD of {a} and {b} is {d}")
		print(f"Euclidean Algorithm Steps: {iterations}\n")

		# Test LCM calculation
		#a = 12
		#b = 20
		#lcmResult = lcm(a, b)
		#print(f"The LCM of {a} and {b} is {lcmResult}\n")
