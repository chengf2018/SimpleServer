#include "stdafx.h"
#include "UserMgr.h"

void CUserMgr::Initialize(CWorkServer *workserver)
{
	//初始化Mysql数据库
	m_SqlConn.SetConnectionInfo("localhost", "test", "123456", "test");
	if (m_SqlConn.Connect() == false)
		printf("MYSQL Connect fail! Error:%s\n", mysql_error(m_SqlConn.GetMysqlObject()));
	else
		printf("MYSQL Connect success!\n");

	//初始化AES KEY
	m_aesKey = "alfemxjawqlamdnf";

	m_WorkServer = workserver;
	assert(m_WorkServer);
}

void CUserMgr::AddUser(evutil_socket_t sockid, CUser *user)
{
	m_OnLineUserListMutex.lock();
	m_OnLineUserList.insert(USERITEM(sockid, user));
	m_OnLineUserListMutex.unlock();
}

CUser* CUserMgr::GetUser(evutil_socket_t sockid)
{
	m_OnLineUserListMutex.lock();
	USERLIST::iterator iter = m_OnLineUserList.find(sockid);

	if (iter == m_OnLineUserList.end())
	{
		m_OnLineUserListMutex.unlock();
		return nullptr;
	}

	m_OnLineUserListMutex.unlock();
	return iter->second;
}

void CUserMgr::DeleteUser(evutil_socket_t sockid)
{
	m_OnLineUserListMutex.lock();
	USERLIST::iterator iter = m_OnLineUserList.find(sockid);
	if (iter == m_OnLineUserList.end())
	{
		m_OnLineUserListMutex.unlock();
		return;
	}

	m_OnLineUserList.erase(iter);
	m_OnLineUserListMutex.unlock();
}

void CUserMgr::LoginSystemHandle(bufferevent *bev, CMemoryStream &packet)
{
	unsigned char nIndex = 0;

	packet >> nIndex;

	switch (nIndex)
	{
	case LoginSystem_cRegister:
		RegisterUser(bev, packet);
		break;
	case LoginSystem_cLogin:
		UserLogin(bev, packet);
		break;
	case LoginSystem_cChangePassWord:
		ChangePassword(bev, packet);
		break;
	}
}

void CUserMgr::RegisterUser(bufferevent *bev, CMemoryStream &packet)
{
	char *username = nullptr;
	char *userpw = nullptr;
	
	username = packet.ReadString();
	userpw = packet.ReadString();

	if (username == nullptr || userpw == nullptr)
		return;

	std::string userpassword = Decode(userpw);

	//检查用户名合法性和密码合法性
	if (!CheckUserName(username))
		return;

	if (!CheckPassword(userpassword.c_str()))
		return;
	
	//....这里可以根据需要加入其他验证,比如手机验证、邮箱验证....//

	
	if (m_SqlConn.isConnected())
	{
		char str[1000] = { 0 };
		//检查用户名是否已经存在
		CSqlCommand sqlCommand(m_SqlConn.GetMysqlObject());

		//select username from user where username = '%s' limit 1
		sprintf_s(str, "select username from user where username = \'%s\' limit 1", username);
		
		if (!sqlCommand.Query(str))
			return;

		SelectResult *ret = sqlCommand.GetResult();
		if (ret->m_nCol > 0)
		{
			CMemoryStream pack(200);
			pack << (unsigned char)cmd_LoginSystem << (unsigned char)LoginSystem_sRegister;
			pack << (unsigned char)0;//失败

			CBaseServer::SendData(bev, pack.GetMemory(), pack.GetWritePos());
			return;
		}

		//向数据库插入数据
		CSqlExec sqlExec(m_SqlConn.GetMysqlObject());
		
		//insert into user (username, password) value ('%s', '%s')
		sprintf_s(str, "insert into user (username, password) value (\'%s\', \'%s\')", username, userpw);
		if (sqlExec.Exec(str))
		{
			printf("insert user %s sucess!\n", username);

			//select * from user where username = '%s' limt 1
			sprintf_s(str, "select userid from user where username = \'%s\' limit 1", username);
			if (!sqlCommand.Query(str))
				return;

			SelectResult *res = sqlCommand.GetResult();
			std::string id = res->GetAt(0, 0);
			int nId = atoi(id.c_str());
			
			//下发注册成功
			CMemoryStream pack(200);
			pack << (unsigned char)cmd_LoginSystem << (unsigned char)LoginSystem_sRegister;
			pack << (unsigned char)1;//成功
			pack << (int)nId;
			pack.WriteString(username);

			CBaseServer::SendData(bev, pack.GetMemory(), pack.GetWritePos());
		}
		else
		{
			printf("insert user %s fail!Error:%s\n", username, mysql_error(m_SqlConn.GetMysqlObject()));
		}
	}

}

void CUserMgr::ChangePassword(bufferevent *bev, CMemoryStream &packet)
{
	char *username = nullptr;
	char *cPassword = nullptr;
	char *cNewPassword = nullptr;

	username = packet.ReadString();
	cPassword = packet.ReadString();
	cNewPassword = packet.ReadString();

	std::string password = Decode(cPassword);
	std::string newPassword = Decode(cNewPassword);

	//用户名的格式和密码的格式是否正确
	if (!CheckUserName(username) || !CheckPassword(password.c_str()) || !CheckPassword(newPassword.c_str()))
		return;

	char str[1000] = { 0 };
	CSqlCommand sqlCommand(m_SqlConn.GetMysqlObject());
	//select * from user where username = '%s' and password = '%s' limit 1
	sprintf_s(str, "select * from user where username = \'%s\' and password = \'%s\' limit 1", username, cPassword);

	if (!sqlCommand.Query(str))
		return;

	SelectResult *res = sqlCommand.GetResult();

	if (res->m_nCol < 1)//没有查询到记录
	{
		CMemoryStream pack(200);
		pack << (unsigned char)cmd_LoginSystem << (unsigned char)LoginSystem_sChangePassWord;
		pack << (unsigned char)0;//修改密码失败

		CBaseServer::SendData(bev, pack.GetMemory(), pack.GetWritePos());
		return;
	}

	//update user set password = '%s' where username = '%s' limit 1
	CSqlExec sqlExec(m_SqlConn.GetMysqlObject());
	sprintf_s(str, "update user set password = \'%s\' where username = \'%s\' limit 1", cNewPassword, username);

	if (sqlExec.Exec(str))
	{
		CMemoryStream pack(200);
		pack << (unsigned char)cmd_LoginSystem << (unsigned char)LoginSystem_sChangePassWord;
		pack << (unsigned char)1;//修改密码成功

		CBaseServer::SendData(bev, pack.GetMemory(), pack.GetWritePos());
	}
	else
	{
		CMemoryStream pack(200);
		pack << (unsigned char)cmd_LoginSystem << (unsigned char)LoginSystem_sChangePassWord;
		pack << (unsigned char)0;//修改密码失败

		CBaseServer::SendData(bev, pack.GetMemory(), pack.GetWritePos());
	}
	
}

void CUserMgr::UserLogin(bufferevent *bev, CMemoryStream &packet)
{
	char *username = nullptr;
	char *passwd = nullptr;

	username = packet.ReadString();
	passwd = packet.ReadString();

	if (username == nullptr || passwd == nullptr)
		return;

	char str[1000] = { 0 };
	CSqlCommand sqlCommand(m_SqlConn.GetMysqlObject());
	//select * from user where username = '%s' and password = '%s' limit 1
	sprintf_s(str, "select * from user where username = \'%s\' and password = \'%s\' limit 1", username, passwd);

	if (!sqlCommand.Query(str))
		return;

	SelectResult *res = sqlCommand.GetResult();

	if (res->m_nCol < 1)//没有查询到记录
	{
		CMemoryStream pack(200);
		pack << (unsigned char)cmd_LoginSystem << (unsigned char)LoginSystem_sLogin;
		pack << (unsigned char)0;//登录失败
		
		CBaseServer::SendData(bev, pack.GetMemory(), pack.GetWritePos());
		return;
	}

	evutil_socket_t sockid = bufferevent_getfd(bev);
	
	if (sockid == -1)
		return;
	
	CUser *user = new CUser(this);
	user->Initialize(bev, res);
	
	AddUser(sockid, user);

	CMemoryStream pack(200);
	pack << (unsigned char)cmd_LoginSystem << (unsigned char)LoginSystem_sLogin;
	pack << (unsigned char)1;//登录成功
	CBaseServer::SendData(bev, pack.GetMemory(), pack.GetWritePos());

	printf("User:%s Login sucess!\n", username);
}

bool CUserMgr::CheckUserName(const char *username)
{
	if (username == nullptr) return false;

	size_t nLen = strlen(username);

	if (nLen < 6 || nLen > 16)
		return false;
	
	if (!((username[0] >= 'A' && username[0] <= 'Z') || (username[0] >= 'a' && username[0] <= 'z')))
		return false;

	for (int i = 1; i < nLen; i++)
	{
		if (!((username[i] >= 'A' && username[i] <= 'Z') || (username[i] >= 'a' && username[i] <= 'z') || (username[i] >= '0' && username[i] <= '9')))
			return false;
	}
	return true;
}

bool CUserMgr::CheckPassword(const char *password)
{
	if (password == nullptr) return false;

	size_t nLen = strlen(password);

	if (nLen < 6 || nLen > 16)
		return false;
	
	for (int i = 0; i < nLen; i++)
	{
		if (password[i] >= 32 && password[i] <= 126)
			continue;

		return false;
	}

	return true;
}

std::string CUserMgr::Decode(std::string decstr)
{
	int outbyte = 0;
	std::string temp = m_base64.Decode(decstr.c_str(), (int)decstr.size(), outbyte);
	return AES::DecryptString(temp, m_aesKey);
}

std::string CUserMgr::EnCode(std::string decstr)
{
	std::string temp = AES::EncryptString(decstr, m_aesKey);	
	return m_base64.Encode((const unsigned char*)temp.c_str(), (int)temp.size());
}

void CUserMgr::KickUser(bufferevent *bev)
{
	evutil_socket_t sockid = bufferevent_getfd(bev);
	if (sockid == -1) return;
	CUser *pUser = GetUser(sockid);
	
	if (pUser != nullptr)
	{
		pUser->OnLoginOut();

		DeleteUser(sockid);
	}

	printf("KickUser sock:%d!\n", sockid);

	m_WorkServer->Close(bev);
}