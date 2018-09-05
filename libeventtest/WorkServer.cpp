#include "stdafx.h"
#include "WorkServer.h"

void CWorkServer::OnProcessMessage(bufferevent *bev, char *message, unsigned short len)
{
	//printf("收到数据:%s\n", message);
	bufferevent_lock(bev);//加锁,以方便处理逻辑业务

	CMemoryStream packet(message, len);

	unsigned char nCmdId = 0;
	packet >> nCmdId;

	printf("CmdId:%d\n", nCmdId);

	if (nCmdId == cmd_LoginSystem)//登录系统单独处理
	{
		m_userMgr.LoginSystemHandle(bev, packet);
		bufferevent_unlock(bev);//return前必须解锁
		return;
	}

	evutil_socket_t sockid = bufferevent_getfd(bev);
	if (sockid == -1)
	{
		bufferevent_unlock(bev);
		return;
	}

	CUser *user = m_userMgr.GetUser(sockid);
	if (user == nullptr)
	{
		bufferevent_unlock(bev);
		return;
	}

	user->ProcessNetMessage(nCmdId, packet);

	bufferevent_unlock(bev);
}

void CWorkServer::OnConnect(bufferevent *bev, char *ip)
{

}

void CWorkServer::OnClose(bufferevent *bev)

{
	evutil_socket_t sockid = bufferevent_getfd(bev);
	if (sockid == -1) return;

	CUser *user = m_userMgr.GetUser(sockid);
	if (user == nullptr) return;

	user->OnLoginOut();

	m_userMgr.DeleteUser(sockid);

}

