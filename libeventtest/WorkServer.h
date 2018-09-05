#pragma once
/*
 * �����׽���
 * ��Ҫ��������������ݰ���
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
	//��ʼ��
	virtual void Initialize()
	{
		Inherited::Initialize();
		m_userMgr.Initialize(this);
	}

	//��ȡ�û�������
	CUserMgr &GetUserMgr() { return m_userMgr; }
protected:
	virtual void OnClose(bufferevent *bev);
	//�������� bev:�¼����, ip:socket��ip��ַ
	virtual void OnConnect(bufferevent *bev, char *ip);
	//����һ����Ϣ
	virtual void OnProcessMessage(bufferevent *bev, char *message, unsigned short len);

private:

	CUserMgr m_userMgr;
};