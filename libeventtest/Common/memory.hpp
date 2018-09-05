#pragma once
//author : chengf
//edited by 2018-4-1
// �ڴ���ģ��

#include <memory.h>
#include <string.h>

//�����ﶨ���ڴ�Ĵ�С
#define MEMORY_INT unsigned short

class CMemoryStream
{
public:
	//�Զ������ڴ�
	CMemoryStream(MEMORY_INT size)
	{
		if (size<=0) return;
		m_pMemory = (char*)malloc(size);
		memset(m_pMemory, 0, size);
		m_nSize = size;
		m_nReadPos = 0;
		m_nWritePos = 0;
		m_isNeedFree = true;
	}

	CMemoryStream(char *memory, MEMORY_INT size)
	{
		if (m_pMemory != NULL && m_isNeedFree)
		{
			free(m_pMemory);
		}
		m_pMemory = memory;
		m_nSize = size;
		m_nReadPos = 0;
		m_nWritePos = 0;
		m_isNeedFree = false;
	}

	~CMemoryStream()
	{
		//��������ڴ����Ľ�����ʽѡ���ͷźͲ��ͷ�
		if (m_pMemory != NULL && m_isNeedFree)
		{
			free(m_pMemory);
		}
	}

	void SetMemory(char *memory, MEMORY_INT size)
	{
		if (m_pMemory != NULL && m_isNeedFree)
		{
			free(m_pMemory);
		}
		m_pMemory = memory;
		m_nSize = size;
		m_nReadPos = 0;
		m_nWritePos = 0;
	}

	//��ȡ�ַ���,�����βû��"\0"�����Զ��ڽ�β��'\0'��ʧ�ܷ���NULL
	char* ReadString()
	{
		if (!m_pMemory) return NULL;

		if (m_nSize - m_nReadPos <= sizeof(unsigned short))return NULL;

		unsigned short strLen = (*(unsigned short *)(m_pMemory + m_nReadPos));

		if (strLen <= 0 || (m_nSize - m_nReadPos < strLen))return NULL;
		char *pos = m_pMemory + m_nReadPos + sizeof(unsigned short);
		m_nReadPos += (sizeof(strLen) + strLen);

		if (*(m_pMemory + m_nReadPos - 1) != '\0')
		{
			*(m_pMemory + m_nReadPos - 1) = '\0';
		}

		return pos;
	}

	//д���ַ���
	void WriteString(const char *str)
	{
		if (!m_pMemory || str == NULL) return;//
		
		unsigned short strLen = (unsigned short)(strlen(str) + 1);
		if (m_nSize - m_nWritePos < strLen + (unsigned short)sizeof(unsigned short)) return;

		memcpy(m_pMemory + m_nWritePos, &strLen, sizeof(strLen));
		memcpy(m_pMemory + m_nWritePos + sizeof(strLen), str, strLen);
		m_nWritePos += (sizeof(strLen) + strLen);
	}


public:
	template<typename T>
	CMemoryStream& operator >> (T& value)
	{
		if (m_pMemory && m_nReadPos + sizeof(T) <= m_nSize)
		{
			value = *(T*)(m_pMemory + m_nReadPos);
			m_nReadPos += sizeof(T);
		}
		return *this;
	}

	template<typename T>
	CMemoryStream& operator << (T value)
	{
		if (m_pMemory && m_nWritePos + sizeof(T) <= m_nSize)
		{
			*(T*)(m_pMemory + m_nWritePos) = value;
			m_nWritePos += sizeof(T);
		}
		return *this;
	}

public:
	//�������Ĵ�С
	inline MEMORY_INT GetSize() { return m_nSize; }

	//��ȡ�ڴ�ָ��
	inline char *GetMemory() { return m_pMemory; }

	//���ö�λ��
	inline void SetReadPos(MEMORY_INT pos)
	{
		if (pos < 0 || pos > m_nSize) return;

		m_nReadPos = pos;
	}

	//����дλ��
	inline void SetWritePos(MEMORY_INT pos)
	{
		if (pos < 0 || pos > m_nSize) return;

		m_nWritePos = pos;
	}

	//��ȡ��λ��
	inline MEMORY_INT GetReadPos() { return m_nReadPos; }

	//��ȡдλ��
	inline MEMORY_INT GetWritePos() { return m_nWritePos; }

private:
	char	*m_pMemory;		//�ڴ��ָ��
	MEMORY_INT m_nReadPos;   //��ȡλ��
	MEMORY_INT m_nWritePos;  //дλ��
	MEMORY_INT m_nSize;      //��С

	bool m_isNeedFree;//����ʱ�Ƿ���Ҫ�ͷ�
};