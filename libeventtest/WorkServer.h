#pragma once
/*
 * 工作套接字
 * 主要负责解析包，传递包等
*/
#include "BaseServer.h"
#include "UserMgr.h"
#include "Common/memory.hpp"

class CBaseServer;

class CWorkServer : public CBaseServer
{
	typedef CBaseServer Inherited;
public:
	CWorkServer() {}
	~CWorkServer() {}

public:
	//初始化
	virtual void Initialize()
	{
		Inherited::Initialize();
		m_userMgr.Initialize(this);
	}

	//获取用户管理器
	CUserMgr &GetUserMgr() { return m_userMgr; }
protected:
	virtual void OnClose(bufferevent *bev);
	//连接上线 bev:事件句柄, ip:socket的ip地址
	virtual void OnConnect(bufferevent *bev, char *ip);
	//处理一条消息
	virtual void OnProcessMessage(bufferevent *bev, char *message, unsigned short len);

private:

	CUserMgr m_userMgr;
};