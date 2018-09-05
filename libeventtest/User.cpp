#include "stdafx.h"
#include "User.h"

CUser::CUser(CUserMgr* mgr)
{
	m_bev = nullptr;
	m_isInit = false;
	m_UserMgr = mgr;
	assert(mgr);
}

CUser::~CUser()
{

}

void CUser::Initialize(bufferevent *bev, SelectResult * res)
{
	m_bev = bev;
	//初始化用户数据
	
	m_userid = (unsigned int)atoi(res->GetAt(0, 0).c_str());
	m_username = res->GetAt(0, 1);

	m_isInit = true;
}


void CUser::ProcessNetMessage(unsigned char nCmdId, CMemoryStream &packet)
{
	switch (nCmdId)
	{
	case cmd_UserInfoSystem:
		m_UserMgr->KickUser(m_bev);
		break;
	}
}

void CUser::SendPacket(CMemoryStream &packet, unsigned short len)
{
	if (m_bev == nullptr) return;

	CBaseServer::SendData(m_bev, packet.GetMemory(), len);
}

void CUser::Save()
{

}

void CUser::OnLoginOut()
{
	Save();
}