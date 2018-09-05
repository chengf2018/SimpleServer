#include "MysqlInterface.h"

CSQLConnection::CSQLConnection()
{
	m_SQLConnection = mysql_init(nullptr);

	mysql_set_character_set(m_SQLConnection, "utf8");//设置字符集为UTF8，才能支持中文
}

CSQLConnection::~CSQLConnection()
{
	Close();
}

void CSQLConnection::SetConnectionInfo(std::string host, std::string user, std::string password, std::string dbname, unsigned int port/* = 3306*/)
{
	if (host.empty() || user.empty() || password.empty() || dbname.empty())
		return;

	m_host = host;
	m_user = user;
	m_password = password;
	m_dbname = dbname;
	m_port = port;
}

bool CSQLConnection::Connect()
{
	if (m_host.empty() || m_user.empty() || m_password.empty() || m_dbname.empty())
		return false;

	m_isConnect = false;

	if (!mysql_real_connect(m_SQLConnection, m_host.c_str(), m_user.c_str(), m_password.c_str(), m_dbname.c_str(), m_port, nullptr, 0))
		return false;

	m_isConnect = true;
	return true;
}

bool CSQLConnection::SelectDb(std::string db)
{
	if (mysql_select_db(m_SQLConnection, db.c_str()))
	{
		if (mysql_errno(m_SQLConnection) != 0)
		{
			Close();
		}
		return false;
	}
	
	return true;
}

void CSQLConnection::Close()
{
	m_isConnect = false;
	mysql_close(m_SQLConnection);
}


/*-------------------------------------------------------------------------------------------------*/

SelectResult* CSqlCommand::GetResult()
{
	MYSQL_RES* res = mysql_store_result(m_SQLConnection);

	if (res == nullptr) return nullptr;

	SelectResult * selectRes = new SelectResult();
	
	MYSQL_ROW row;
	unsigned int nRCount = mysql_num_fields(res);
	unsigned int nCCount = 0;
	while (row = mysql_fetch_row(res))
	{
		std::vector<std::string> tempList;

		for (unsigned int i = 0; i < nRCount; i++)
			tempList.push_back(row[i]);

		selectRes->m_datalist.push_back(tempList);
		nCCount++;
	}
	selectRes->m_nRow = nRCount;
	selectRes->m_nCol = nCCount;
	mysql_free_result(res);

	m_reslist.push_back(selectRes);
	return selectRes;
}

void CSqlCommand::FreeResult(SelectResult* res)
{
	std::vector<SelectResult*>::iterator iter = m_reslist.begin();
	std::vector<SelectResult*>::iterator iterEnd = m_reslist.end();
	for (; iter != iterEnd; iter++)
	{
		if (res == *iter)
		{
			m_reslist.erase(iter);
			break;
		}
	}
	delete res;
}

void CSqlCommand::FreeList()
{
	std::vector<SelectResult*>::iterator iter = m_reslist.begin();
	std::vector<SelectResult*>::iterator iterEnd = m_reslist.end();
	for (; iter != iterEnd; iter++)
	{
		delete *iter;
	}
}