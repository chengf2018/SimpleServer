#pragma once

#include <string>
#include "Common/memory.hpp"
#include "BaseServer.h"
#include "UserMgr.h"
/*
 * �û���
 * ����ʱ�贫���������ָ��
*/
using namespace std;

class CUserMgr;

class CUser
{
public:
	CUser(CUserMgr* mgr);
	~CUser();

public:
	//��ʼ��
	void Initialize(bufferevent *bev, SelectResult* res);

	//����������Ϣ
	void ProcessNetMessage(unsigned char nCmdId, CMemoryStream &packet);

	//�������ݰ� ���͵Ĵ�С��lenָ���Ĵ�С������packet�Ĵ�С
	void SendPacket(CMemoryStream &packet, unsigned short len);

	//�����û�����
	void Save();

	//�û�����
	void OnLoginOut();

	//�Ƿ��ʼ��
	inline bool isInitialize() { return m_isInit; }
private:
	/*User basic data*/
	unsigned int	m_userid;//�˻�id
	string			m_username;//�˻���

	bufferevent *m_bev;//buffereventָ��

	CUserMgr* m_UserMgr;//�û������Ĺ�����

	bool   m_isInit;//�Ƿ��ʼ��
};