#pragma once
//author : chengf
//edited by 2018-4-1
// 内存流模块

#include <memory.h>
#include <string.h>

//在这里定义内存的大小
#define MEMORY_INT unsigned short

class CMemoryStream
{
public:
	//自动申请内存
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
		//这里根据内存流的建立方式选择释放和不释放
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

	//读取字符串,如果结尾没有"\0"，则自动在结尾加'\0'，失败返回NULL
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

	//写入字符串
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
	//返回流的大小
	inline MEMORY_INT GetSize() { return m_nSize; }

	//获取内存指针
	inline char *GetMemory() { return m_pMemory; }

	//设置读位置
	inline void SetReadPos(MEMORY_INT pos)
	{
		if (pos < 0 || pos > m_nSize) return;

		m_nReadPos = pos;
	}

	//设置写位置
	inline void SetWritePos(MEMORY_INT pos)
	{
		if (pos < 0 || pos > m_nSize) return;

		m_nWritePos = pos;
	}

	//获取读位置
	inline MEMORY_INT GetReadPos() { return m_nReadPos; }

	//获取写位置
	inline MEMORY_INT GetWritePos() { return m_nWritePos; }

private:
	char	*m_pMemory;		//内存块指针
	MEMORY_INT m_nReadPos;   //读取位置
	MEMORY_INT m_nWritePos;  //写位置
	MEMORY_INT m_nSize;      //大小

	bool m_isNeedFree;//析构时是否需要释放
};