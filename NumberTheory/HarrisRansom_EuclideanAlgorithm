# Harris Ransom
# Euclidean GCD Algorithm
# 9/18/2024

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