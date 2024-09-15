# Harris Ransom
# Euler's Theorem/Totient Function
# 9/15/2024

# Imports
from EuclideanAlgorithm import gcdRecursive

# Euler Totient Function
def totient(n):
	count = 0
	for i in range (1, n+1):
		if (gcdRecursive(n, i) == 1):
			count += 1
	return count

# Test Euler's Theorem
# a ^ (phi(n)) = 1 (mod n)
def eulersTheorem(a, n):
	if (gcdRecursive(a, n) != 1):
		return False, 0
	else:
		return True, totient(n)
# MAIN
if __name__ == "__main__":
	a = int(input("Exponent base: "))
	n = int(input("Modular base: "))
	
	isValid, phi = eulersTheorem(a, n)
	if (isValid):
		print(f"Euler Totient Function phi({n}): {phi}")
		print(f"a^(phi(n)): {a**phi} == 1 (mod {n})")
	else:
		print(f"{a} and {n} are not relatively coprime")
