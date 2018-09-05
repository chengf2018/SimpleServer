#pragma once

#include <string>
#include "mysql.h"
#include <mutex>
#include <vector>

/*SQL������*/
class CSQLConnection
{
public:
	CSQLConnection();
	~CSQLConnection();

	//����������Ϣ
	void SetConnectionInfo(std::string host, std::string user, std::string password, std::string dbname, unsigned int port = 3306);
	//����
	bool Connect();
	//�ر�
	void Close();
	//��ȡ����״̬
	inline bool isConnected() { return m_isConnect; }
	//ѡ�����ݿ�
	bool SelectDb(std::string db);
	//��ȡsql���Ӷ���
	inline MYSQL *GetMysqlObject() { return m_SQLConnection; }

public:
	/*---������Ϣ---*/
	std::string m_host;
	std::string m_user;
	std::string m_dbname;
	unsigned int m_port;
	/*--------------*/
private:
	MYSQL * m_SQLConnection; //���Ӷ���

	std::mutex m_mutex; // ��
	
	std::string m_password; //��������

	bool m_isConnect = false; //����״̬
};
/*--------------------------------------------------------------------------------*/
/*Sqlִ���� ����ִ�в���Ҫ���ؽ����sql���*/
class CSqlExec
{
public:
	CSqlExec(){}
	CSqlExec(MYSQL * pConn) { m_SQLConnection = pConn; }
	~CSqlExec() {}

	inline void SetConnectObject(MYSQL * pConn) { m_SQLConnection = pConn; }

	inline bool Exec(std::string command)
	{
		if (!m_SQLConnection || command.empty()) return false;

		if (mysql_query(m_SQLConnection, command.c_str()))
		{
			return false;
		}
		return true;
	}
protected:
	MYSQL * m_SQLConnection = nullptr;
};
/*--------------------------------------------------------------------------------*/
/*��ȫsql��ѯ��*/
class CSqlCommand : public CSqlExec
{
	typedef CSqlExec Father;
public:
	CSqlCommand() {}
	CSqlCommand(MYSQL * pConn) : Father(pConn) {}
	
	~CSqlCommand()
	{
		FreeList();
	}

	/*ִ�в�ѯ ʹ��Query����ʹ��GetResult����ȡ���*/
	inline bool Query(std::string command)
	{
		return Exec(command);
	}
	/*��ȡ��ѯ���*/
	class SelectResult * GetResult();
	/*�ͷŵ�����ѯ���*/
	void FreeResult(SelectResult* res);
	/*�ͷŲ�ѯ����б�*/
	void FreeList();
private:
	std::vector<SelectResult*> m_reslist; //���ڱ����ѯ������б������������ͷ��ڴ�ռ�
};

/*--------------------------------------------------------------------------------*/
/*���ؽ����*/
class SelectResult
{
public:
	inline std::string GetAt(unsigned int col, unsigned int row)
	{
		if (row < 0 || row > m_nRow || col < 0 || col > m_nCol)
			return "[Error]";

		return m_datalist[col][row];
	}
public:
	unsigned int m_nRow = 0;//����
	unsigned int m_nCol = 0;//����
	std::vector<std::vector<std::string>> m_datalist;
};