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
	// �����ı� src�����ܵ��ı���key����(16�ֽ�����)�� �ɹ����ؼ��ܺ���ı��� ʧ�ܷ���""
	std::string EncryptString(std::string &src, std::string &key);
	
	// �����ı� src�����ܵ��ı�, srclenԭ�ı�����, [out] tar���ؼ��ܺ���ı�, tar�Ĵ�С, outlen�����С key���루16�ֽڣ����ɹ�����true ʧ�ܷ���false
	bool EncryptString(const char *src, unsigned int srclen, char *tar, unsigned int tarlen, unsigned int *outlen, const char *key);

	// �����ı� src�����ܵ��ı�, key����(16�ֽ�����)�� �ɹ����ؽ��ܺ���ı��� ʧ�ܷ���""
	std::string DecryptString(std::string &src, std::string &key);
	
	// �����ı� src�����ܵ��ı�, srclenԭ�ı�����, [out] tar���ؽ��ܺ���ı�, tar�Ĵ�С, outlen�����С key���루16�ֽڣ����ɹ�����true ʧ�ܷ���false
	bool DecryptString(const char *src, unsigned int srclen, char *tar, unsigned int tarlen, unsigned int *outlen, const char *key);
};

	