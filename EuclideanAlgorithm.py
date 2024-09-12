# Harris Ransom
# Euclidean GCD Algorithm
# 9/11/2024

# Imports


# Euclidean Algorithm
def gcd(a, b):
	if b == 0:
		return a
	elif a == b:
		return a
	else:
		remainder = a % b
		return gcd(b, remainder)

# MAIN
if __name__ == "__main__":
	while (True):
		# Get input
		a = int(input("Enter a: "))
		b = int(input("Enter b: "))

		# Calculate GCD
		d = gcd(a, b)
		print(f"The GCD of {a} and {b} is {d}\n")
