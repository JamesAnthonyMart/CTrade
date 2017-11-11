#pragma once

#include <string>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>

class HashUtil
{
public:
	static std::string sha512(std::string p_SecretKey, std::string p_Message)
	{
		//Todo determine way to be more memory efficient
		char key[10000];
		char data[1024];
		strcpy_s(key, p_SecretKey.c_str());
		strncpy_s(data, p_Message.c_str(), sizeof(data));
		data[sizeof(data) - 1] = 0;


		unsigned char* digest;

		//============================================
		//Example extract from http://www.askyb.com/cpp/openssl-hmac-hasing-example-in-cpp/
		//askyb on February, 26th 2013 in C++ OpenSSL
		//=============================================
		// Using sha1 hash engine here.
		// You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
		digest = HMAC(EVP_sha512(), key, static_cast<int>(strlen(key)), (unsigned char*)data, strlen(data), NULL, NULL);

		//===== When debugging ========================
		//printf("HMAC digest:           ");
		//for (int i = 0; i != SHA512_DIGEST_LENGTH; i++)
		//	printf("%02hhx", digest[i]);
		//printf("\n");
		//===== When debugging ========================

		//SHA512 produces a 64-byte hash value which rendered as 128 characters.
		// Change the length accordingly with your choosen hash engine
		size_t bufferSize = SHA512_DIGEST_LENGTH * 2 + 1;
		char mdString[SHA512_DIGEST_LENGTH * 2 + 1];
		int tempint = 0;
		for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
		{	//sprintf and snprintf automatically append '\0'
			//sprintf(&mdString[i * 2], "%02hhx", digest[i]);	//works
			snprintf(&mdString[i * 2], sizeof(mdString), "%02hhx", digest[i]);
		}

		//===== When debugging ========================
		//printf("\Converted HMAC digest: %s\n", mdString);
		//printf("Size is %u characters long.\n", (unsigned)strlen(mdString));
		//printf("And tempint = %d", tempint);
		//===== When debugging ========================

		std::string output = mdString;
		return output;
	}


};