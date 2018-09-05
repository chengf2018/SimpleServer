#pragma once

#include <string>
#include "mysql.h"
#include <mutex>
#include <vector>

/*SQL连接类*/
class CSQLConnection
{
public:
	CSQLConnection();
	~CSQLConnection();

	//设置连接信息
	void SetConnectionInfo(std::string host, std::string user, std::string password, std::string dbname, unsigned int port = 3306);
	//连接
	bool Connect();
	//关闭
	void Close();
	//获取连接状态
	inline bool isConnected() { return m_isConnect; }
	//选择数据库
	bool SelectDb(std::string db);
	//获取sql连接对象
	inline MYSQL *GetMysqlObject() { return m_SQLConnection; }

public:
	/*---连接信息---*/
	std::string m_host;
	std::string m_user;
	std::string m_dbname;
	unsigned int m_port;
	/*--------------*/
private:
	MYSQL * m_SQLConnection; //连接对象

	std::mutex m_mutex; // 锁
	
	std::string m_password; //连接密码

	bool m_isConnect = false; //连接状态
};
/*--------------------------------------------------------------------------------*/
/*Sql执行类 用于执行不需要返回结果的sql语句*/
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
/*安全sql查询类*/
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

	/*执行查询 使用Query必须使用GetResult来获取结果*/
	inline bool Query(std::string command)
	{
		return Exec(command);
	}
	/*获取查询结果*/
	class SelectResult * GetResult();
	/*释放单条查询结果*/
	void FreeResult(SelectResult* res);
	/*释放查询结果列表*/
	void FreeList();
private:
	std::vector<SelectResult*> m_reslist; //用于保存查询结果的列表，在类析构后释放内存空间
};

/*--------------------------------------------------------------------------------*/
/*返回结果类*/
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
	unsigned int m_nRow = 0;//行数
	unsigned int m_nCol = 0;//列数
	std::vector<std::vector<std::string>> m_datalist;
};