# Harris Ransom
# Infonet Security HW2
# One Time Pad
# 10/2/2024

# One-Time Pad XOR Encrypt
def otpEncrypt(m, k):
	ciphertext = []
	if (len(m) != len(k)):
		raise ValueError("Message and key are not the same length")
	
	for i in range(0, len(m)):
		c = ord(m[i]) ^ ord(k[i])
		ciphertext.append(chr(c))
	return ciphertext

# One-Time Pad XOR Decrypt
def otpDecrypt(c, k):
	plaintext = []
	if (len(c) != len(k)):
		raise ValueError("Ciphertext and key are not the same length")

	for i in range(0, len(c)):
		p = ord(c[i]) ^ ord(k[i])
		plaintext.append(chr(p))
	return "".join(plaintext)

# MAIN
if __name__ == "__main__":
	plaintext = "helloAlice!"
	key = "$%wB+=?Qz?4"

	# Encrypt plaintext
	ciphertext = otpEncrypt(plaintext, key)
	print(f"Ciphertext: {ciphertext}")

	# Decrypt ciphertext
	decrypted = otpDecrypt(ciphertext, key)
	print(f"Decrypted Message: {decrypted}")
	if (decrypted == plaintext):
		print("Plaintext successfully encrypted and decrypted!")
	else:
		print("Error in encryption/decryption process!")