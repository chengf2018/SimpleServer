#pragma once

#include "BaseServer.h"
#include <map>
#include "Common/MysqlInterface.h"
#include "User.h"
/* 用户管理器
 * 用于管理用户
*/

const unsigned int MAX_USER_COUNT = 1000; //最大用户数

class CBaseServer;
class CWorkServer;
class CUser;

class CUserMgr
{
	typedef std::map<evutil_socket_t, CUser*> USERLIST;
	typedef std::pair<evutil_socket_t, CUser*> USERITEM;
public:
	//初始化
	void Initialize(CWorkServer *workserver);

	//登录系统处理
	void LoginSystemHandle(bufferevent *bev, CMemoryStream &packet);

	//注册用户
	void RegisterUser(bufferevent *bev, CMemoryStream &packet);

	//用户登录
	void UserLogin(bufferevent *bev, CMemoryStream &packet);

	//修改密码
	void ChangePassword(bufferevent *bev, CMemoryStream &packet);



public:
	/*
	* 在线用户列表操作
	*/
	//添加用户到列表
	void AddUser(evutil_socket_t sockid, CUser *user);
	//获取用户指针
	CUser *GetUser(evutil_socket_t sockid);
	//从在线列表中删除用户
	void DeleteUser(evutil_socket_t sockid);


	/*
	* 账户格式检测以及编码解码功能
	*/
	//检查用户名的合法性
	bool CheckUserName(const char *username);
	//检查密码合法性
	bool CheckPassword(const char *password);
	//解码解密
	std::string Decode(std::string decstr);
	//编码加密
	std::string EnCode(std::string decstr);

	/*
	* 其他操作
	*/
	//踢用户下线
	void KickUser(bufferevent *bev);

private:
	// sql 连接
	CSQLConnection m_SqlConn;

	//Base64编解码
	ZBase64 m_base64;
	
	//AES KEY
	std::string m_aesKey;

	//在线用户列表
	USERLIST m_OnLineUserList;
	//在线用户列表锁
	std::mutex m_OnLineUserListMutex;

	CWorkServer *m_WorkServer;
};