#pragma once

#include "BaseServer.h"
#include <map>
#include "Common/MysqlInterface.h"
#include "User.h"
/* �û�������
 * ���ڹ����û�
*/

const unsigned int MAX_USER_COUNT = 1000; //����û���

class CBaseServer;
class CWorkServer;
class CUser;

class CUserMgr
{
	typedef std::map<evutil_socket_t, CUser*> USERLIST;
	typedef std::pair<evutil_socket_t, CUser*> USERITEM;
public:
	//��ʼ��
	void Initialize(CWorkServer *workserver);

	//��¼ϵͳ����
	void LoginSystemHandle(bufferevent *bev, CMemoryStream &packet);

	//ע���û�
	void RegisterUser(bufferevent *bev, CMemoryStream &packet);

	//�û���¼
	void UserLogin(bufferevent *bev, CMemoryStream &packet);

	//�޸�����
	void ChangePassword(bufferevent *bev, CMemoryStream &packet);



public:
	/*
	* �����û��б����
	*/
	//����û����б�
	void AddUser(evutil_socket_t sockid, CUser *user);
	//��ȡ�û�ָ��
	CUser *GetUser(evutil_socket_t sockid);
	//�������б���ɾ���û�
	void DeleteUser(evutil_socket_t sockid);


	/*
	* �˻���ʽ����Լ�������빦��
	*/
	//����û����ĺϷ���
	bool CheckUserName(const char *username);
	//�������Ϸ���
	bool CheckPassword(const char *password);
	//�������
	std::string Decode(std::string decstr);
	//�������
	std::string EnCode(std::string decstr);

	/*
	* ��������
	*/
	//���û�����
	void KickUser(bufferevent *bev);

private:
	// sql ����
	CSQLConnection m_SqlConn;

	//Base64�����
	ZBase64 m_base64;
	
	//AES KEY
	std::string m_aesKey;

	//�����û��б�
	USERLIST m_OnLineUserList;
	//�����û��б���
	std::mutex m_OnLineUserListMutex;

	CWorkServer *m_WorkServer;
};