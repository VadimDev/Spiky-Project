// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#pragma warning(disable:4996)

#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include <openssl/bn.h>
#include <string>
#include <google/protobuf/message.h>
#include "Crypto.generated.h"

struct keys
{
	char * p;
	char * g;
	char * pubKey;
	char * privKey;
};

UCLASS()
class SPIKY_CLIENT_API UCrypto : public UObject
{
	GENERATED_BODY()

public:
	// DiffieHellman
	// Initializing p and g, depending on the desired key length 
	static DH *get_dh(int size); // 512 or 1024
	// Creates and saves a set of keys: p,g, private key, public key
	static keys Generate_KeysSet_DH();
	// DH instance
	static DH * client;
	// Creates a shared secret based on the incoming key, returns a string in Base64
	static std::string Generate_SecretKey_DH(std::string str);

	// Base64
	// Calculates the length of the decoded string
	static size_t CalcDecodeLength(const char* b64input);
	// Base64 encoding/decoding
	static size_t Base64Decode(char* b64message, unsigned char** buffer, size_t* length);
	static std::string Base64Encode(char *decoded_bytes, size_t decoded_length);

	// Sha256
	// Get a hash
	static std::string SHA256(const void *data, size_t data_len);

	// AES_ecb_128
	// Different versions for any data, key length 16
	static int AES_ECB_Encrypt(unsigned char *source, int source_len, unsigned char *key, unsigned char *cipher);
	static int AES_ECB_Decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *plaintext);
	static std::string Encrypt(std::string source, std::string key);
	static std::string Decrypt(std::string cipher, std::string key);
	static std::string EncryptProto(google::protobuf::Message * message, std::string key);

private:

	static void handleErrors(void);

};