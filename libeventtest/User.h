#pragma once

#include <string>
#include "Common/memory.hpp"
#include "BaseServer.h"
#include "UserMgr.h"
/*
 * 用户类
 * 构造时需传入管理器的指针
*/
using namespace std;

class CUserMgr;

class CUser
{
public:
	CUser(CUserMgr* mgr);
	~CUser();

public:
	//初始化
	void Initialize(bufferevent *bev, SelectResult* res);

	//处理网络消息
	void ProcessNetMessage(unsigned char nCmdId, CMemoryStream &packet);

	//发送数据包 发送的大小是len指定的大小，不是packet的大小
	void SendPacket(CMemoryStream &packet, unsigned short len);

	//保存用户数据
	void Save();

	//用户下线
	void OnLoginOut();

	//是否初始化
	inline bool isInitialize() { return m_isInit; }
private:
	/*User basic data*/
	unsigned int	m_userid;//账户id
	string			m_username;//账户名

	bufferevent *m_bev;//bufferevent指针

	CUserMgr* m_UserMgr;//用户所属的管理器

	bool   m_isInit;//是否初始化
};