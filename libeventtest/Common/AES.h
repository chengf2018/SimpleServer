#pragma once

#include <stdio.h>
#include <string>

//using namespace std;

class __declspec(dllexport) CAes
{
public:
	CAes(unsigned char* key);
	virtual ~CAes();
	unsigned char* Cipher(unsigned char* input);
	unsigned char* InvCipher(unsigned char* input);
	void* Cipher(void* input, int length=0);
	void* InvCipher(void* input, int length);

private:
	unsigned char Sbox[256];
	unsigned char InvSbox[256];
	unsigned char w[11][4][4];

	void KeyExpansion(unsigned char* key, unsigned char w[][4][4]);
	unsigned char FFmul(unsigned char a, unsigned char b);

	void SubBytes(unsigned char state[][4]);
	void ShiftRows(unsigned char state[][4]);
	void MixColumns(unsigned char state[][4]);
	void AddRoundKey(unsigned char state[][4], unsigned char k[][4]);

	void InvSubBytes(unsigned char state[][4]);
	void InvShiftRows(unsigned char state[][4]);
	void InvMixColumns(unsigned char state[][4]);
};


namespace AES
{
	// 加密文本 src欲加密的文本，key密码(16字节密码)， 成功返回加密后的文本， 失败返回""
	std::string EncryptString(std::string &src, std::string &key);
	
	// 加密文本 src欲加密的文本, srclen原文本长度, [out] tar返回加密后的文本, tar的大小, outlen输出大小 key密码（16字节），成功返回true 失败返回false
	bool EncryptString(const char *src, unsigned int srclen, char *tar, unsigned int tarlen, unsigned int *outlen, const char *key);

	// 解密文本 src欲解密的文本, key密码(16字节密码)， 成功返回解密后的文本， 失败返回""
	std::string DecryptString(std::string &src, std::string &key);
	
	// 解密文本 src欲解密的文本, srclen原文本长度, [out] tar返回解密后的文本, tar的大小, outlen输出大小 key密码（16字节），成功返回true 失败返回false
	bool DecryptString(const char *src, unsigned int srclen, char *tar, unsigned int tarlen, unsigned int *outlen, const char *key);
};

	