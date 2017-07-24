// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "Crypto.h"

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4267)

// Base64, AES
#include <string>
#include <assert.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/err.h>
// Sha256
#include <sstream>
#include <iomanip>
// DH
#include <openssl/crypto.h>
#include <openssl/dh.h>

#include <memory>
#include "Config.h"

using namespace std;

DH * UCrypto::get_dh(int size)
{
	static unsigned char dh512_p[] = {
		0xDA, 0x58, 0x3C, 0x16, 0xD9, 0x85, 0x22, 0x89, 
		0xD0, 0xE4, 0xAF, 0x75, 0x6F, 0x4C, 0xCA, 0x92, 
		0xDD, 0x4B, 0xE5, 0x33, 0xB8, 0x04, 0xFB, 0x0F,
		0xED, 0x94, 0xEF, 0x9C, 0x8A, 0x44, 0x03, 0xED, 
		0x57, 0x46, 0x50, 0xD3, 0x69, 0x99, 0xDB, 0x29, 
		0xD7, 0x76, 0x27, 0x6B, 0xA2, 0xD3, 0xD4, 0x12,
		0xE2, 0x18, 0xF4, 0xDD, 0x1E, 0x08, 0x4C, 0xF6, 
		0xD8, 0x00, 0x3E, 0x7C, 0x47, 0x74, 0xE8, 0x33
	};

	static unsigned char dh1024_p[] = {
		0xF4, 0x88, 0xFD, 0x58, 0x4E, 0x49, 0xDB, 0xCD, 
		0x20, 0xB4, 0x9D, 0xE4, 0x91, 0x07, 0x36, 0x6B, 
		0x33, 0x6C, 0x38, 0x0D, 0x45, 0x1D, 0x0F, 0x7C,
		0x88, 0xB3, 0x1C, 0x7C, 0x5B, 0x2D, 0x8E, 0xF6, 
		0xF3, 0xC9, 0x23, 0xC0, 0x43, 0xF0, 0xA5, 0x5B, 
		0x18, 0x8D, 0x8E, 0xBB, 0x55, 0x8C, 0xB8, 0x5D,
		0x38, 0xD3, 0x34, 0xFD, 0x7C, 0x17, 0x57, 0x43, 
		0xA3, 0x1D, 0x18, 0x6C, 0xDE, 0x33, 0x21, 0x2C, 
		0xB5, 0x2A, 0xFF, 0x3C, 0xE1, 0xB1, 0x29, 0x40,
		0x18, 0x11, 0x8D, 0x7C, 0x84, 0xA7, 0x0A, 0x72, 
		0xD6, 0x86, 0xC4, 0x03, 0x19, 0xC8, 0x07, 0x29, 
		0x7A, 0xCA, 0x95, 0x0C, 0xD9, 0x96, 0x9F, 0xAB,
		0xD0, 0x0A, 0x50, 0x9B,	0x02, 0x46, 0xD3, 0x08, 
		0x3D, 0x66, 0xA4, 0x5D, 0x41, 0x9F, 0x9C, 0x7C, 
		0xBD, 0x89, 0x4B, 0x22, 0x19, 0x26, 0xBA, 0xAB,
		0xA2, 0x5E, 0xC3, 0x55, 0xE9, 0x2F, 0x78, 0xC7
	};

	static unsigned char dh_g[] = {
		0x02,
	};

	DH *dh;

	if (size == 512)
	{
		if ((dh = DH_new()) == NULL) return(NULL);
		dh->p = BN_bin2bn(dh512_p, sizeof(dh512_p), NULL);
		dh->g = BN_bin2bn(dh_g, sizeof(dh_g), NULL);
	}
	else
	{
		if ((dh = DH_new()) == NULL) return(NULL);
		dh->p = BN_bin2bn(dh1024_p, sizeof(dh1024_p), NULL);
		dh->g = BN_bin2bn(dh_g, sizeof(dh_g), NULL);
	}

	if ((dh->p == NULL) || (dh->g == NULL))
	{
		DH_free(dh); return(NULL);
	}
	return(dh);
}

//char * UOpenSSLCrypto::private_key_dh = "";
DH * UCrypto::client = get_dh(512); // DH_new(); // <- use pregenegate P/G or generate manualy (cpu heavy task)

keys UCrypto::Generate_KeysSet_DH()
{
	//DH_generate_parameters_ex(client, 512, DH_GENERATOR_2, NULL); //  generate P/G manualy
	// if you generate P/G manualy you also must send P/G to server
	DH_generate_key(client);

	keys keys_set;

	keys_set.p = BN_bn2dec(client->p);
	keys_set.g = BN_bn2dec(client->g);
	keys_set.pubKey = BN_bn2dec(client->pub_key);
	keys_set.privKey = BN_bn2dec(client->priv_key);

	return keys_set;
}

string UCrypto::Generate_SecretKey_DH(string str)
{
	BIGNUM *pub_bob_key = BN_new();
	BN_dec2bn(&pub_bob_key, str.c_str());

	unsigned char * dh_secret = (unsigned char*)OPENSSL_malloc(sizeof(unsigned char) * (DH_size(client)));

	DH_compute_key(dh_secret, pub_bob_key, client);

	return Base64Encode((char*)dh_secret, sizeof(unsigned char) * (DH_size(client)));
}

size_t UCrypto::CalcDecodeLength(const char* b64input) { //Calculates the length of a decoded string
	size_t len = strlen(b64input),
		padding = 0;

	if (b64input[len - 1] == '=' && b64input[len - 2] == '=') //last two chars are =
		padding = 2;
	else if (b64input[len - 1] == '=') //last char is =
		padding = 1;

	return (len * 3) / 4 - padding;
}

size_t UCrypto::Base64Decode(char* b64message, unsigned char** buffer, size_t* length) { //Decodes a base64 encoded string
	BIO *bio, *b64;

	int decodeLen = CalcDecodeLength(b64message);
	*buffer = (unsigned char*)malloc(decodeLen + 1);
	(*buffer)[decodeLen] = '\0';

	bio = BIO_new_mem_buf(b64message, -1);
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
	*length = BIO_read(bio, *buffer, strlen(b64message));
	assert(*length == decodeLen); //length should equal decodeLen, else something went horribly wrong
	BIO_free_all(bio);

	return (0); //success
}

string UCrypto::Base64Encode(char *decoded_bytes, size_t decoded_length)
{
	int x;
	BIO *bioMem, *b64;
	BUF_MEM *bufPtr;

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bioMem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bioMem);

	BIO_write(b64, decoded_bytes, decoded_length);
	x = BIO_flush(b64);
	if (x < 1) {
		BIO_free_all(b64);
		return NULL;
	}

	BIO_get_mem_ptr(b64, &bufPtr);

	string buff(bufPtr->data, bufPtr->length);

	BIO_free_all(b64);

	return buff;
}

/*
// USAGE EXAMPLE
//Encode To Base64
char* base64EncodeOutput, *text = "Hello World";

char* inbase = OpenSSL_Base64::Base64Encode(text, strlen((char*)text));

cout << inbase << endl;

//Decode From Base64
unsigned char* base64DecodeOutput;
size_t test;
OpenSSL_Base64::Base64Decode(inbase, &base64DecodeOutput, &test);

cout << base64DecodeOutput << endl;
*/

string UCrypto::SHA256(const void * data, size_t data_len)
{
	EVP_MD_CTX mdctx;
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len;

	EVP_DigestInit(&mdctx, EVP_sha256());
	EVP_DigestUpdate(&mdctx, data, (size_t)data_len);
	EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
	EVP_MD_CTX_cleanup(&mdctx);

	std::stringstream s;
	s.fill('0');

	for (size_t i = 0; i < md_len; ++i)
		s << std::setw(2) << std::hex << (unsigned short)md_value[i];

	return s.str();
}

int UCrypto::AES_ECB_Encrypt(unsigned char * plaintext, int plaintext_len, unsigned char * key, unsigned char * ciphertext)
{
	EVP_CIPHER_CTX *ctx;

	int len;

	int ciphertext_len;

	/* Create and initialise the context */
	ctx = EVP_CIPHER_CTX_new();

	if (!ctx) handleErrors();

	/* Initialise the encryption operation. IMPORTANT - ensure you use a key size appropriate for your cipher
	 * In this we are using 128 bit AES (i.e. a 128 bit key).
	 */
	if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL)) handleErrors();

	/* Provide the message to be encrypted, and obtain the encrypted output.
	* EVP_EncryptUpdate can be called multiple times if necessary
	*/
	if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) handleErrors();

	ciphertext_len = len;

	/* Finalise the encryption. Further ciphertext bytes may be written at this stage. */
	if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();

	ciphertext_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return ciphertext_len;
}

int UCrypto::AES_ECB_Decrypt(unsigned char * ciphertext, int ciphertext_len, unsigned char * key, unsigned char * plaintext)
{
	EVP_CIPHER_CTX *ctx;

	int len;

	int plaintext_len;

	/* Create and initialise the context */
	ctx = EVP_CIPHER_CTX_new();

	if (!ctx) handleErrors();

	if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL))
		handleErrors();

	/* Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary
	 */
	if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) handleErrors();

	plaintext_len = len;

	/* Finalise the decryption. Further plaintext bytes may be written at this stage. */
	if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();

	plaintext_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return plaintext_len;
}

std::string UCrypto::Encrypt(string source, string key)
{
	if (Config::bEnableCrypt)
	{
		string tmpkey = key.substr(0, 16);
		unsigned char * key_c = (unsigned char*)strcpy((char*)malloc(tmpkey.length() + 1), tmpkey.c_str());

		auto cipher = make_unique<unsigned char[]>(source.length() * 2 + 8);

		unsigned char * source_c = (unsigned char*)source.c_str();

		size_t cipherLen = AES_ECB_Encrypt(source_c, strlen((char*)source_c), key_c, cipher.get());

		string cipher_str((char*)cipher.get(), cipherLen);

		free(key_c);
		return cipher_str;
	}
	else
	{
		return source;
	}
}

std::string UCrypto::Decrypt(std::string cipher, std::string key)
{
	if (Config::bEnableCrypt)
	{
		string tmpkey = key.substr(0, 16);
		unsigned char * key_c = (unsigned char*)strcpy((char*)malloc(tmpkey.length() + 1), tmpkey.c_str());

		auto source = make_unique<unsigned char[]>(cipher.length() * 2);

		unsigned char * cipher_c = (unsigned char*)cipher.c_str();

		size_t decryptLen = AES_ECB_Decrypt(cipher_c, cipher.length(), key_c, source.get());

		string decrypt_str((char*)source.get(), decryptLen);

		free(key_c);
		return decrypt_str;
	}
	else
	{
		return cipher;
	}
}

std::string UCrypto::EncryptProto(google::protobuf::Message * message, std::string key)
{
	int size = message->ByteSize();
	auto proto_arr = make_unique<unsigned char[]>(size);
	message->SerializeToArray(proto_arr.get(), size);

	if (Config::bEnableCrypt)
	{
		string tmpkey = key.substr(0, 16);
		unsigned char * key_c = (unsigned char*)strcpy((char*)malloc(tmpkey.length() + 1), tmpkey.c_str());

		auto cipher = make_unique<unsigned char[]>(size * 2 + 8);

		unsigned char * source_c = (unsigned char*)proto_arr.get();

		size_t cipherLen = AES_ECB_Encrypt(source_c, size, key_c, cipher.get());

		string cipher_str((char*)cipher.get(), cipherLen);

		free(key_c);
		return cipher_str;
	}
	else
	{
		string cipher_str((char*)proto_arr.get(), size);
		return cipher_str;
	}
}

void UCrypto::handleErrors(void)
{
	ERR_print_errors_fp(stderr);
	abort();
}
