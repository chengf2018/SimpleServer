#pragma once
/*
	BaseServer ����������
	ʹ��ʱֻ��Ҫ����һ����̳�CBaseServer�����̳�OnClose��OnConnect��OnProcessMessage�෽������
	
	ʹ��ʾ����
	CServer : public CBaseServer
	{
	public:
		virtual void Initialize()
		{
			Inherited::Initialize();
		}
	protected:
		virtual void OnClose(bufferevent *bev);
		virtual void OnConnect(bufferevent *bev, char *ip);
		virtual void OnProcessMessage(bufferevent *bev, char *message, unsigned short len);
	}

	...

	CServer server;
	server.Initialize();
	server.SetPort(4399);
	server.Start(true);
*/



#define MAX_BUFFER_LEN 65535 //���ݱ�����󳤶�
#define PACKET_HEAD_SIZE sizeof(PACKETHEAD) //��ͷ��С

//��ͷ���壬Ŀǰֻ��һ������С��������չ
#pragma pack(push,1)
typedef struct _packethead
{
	unsigned short size; //���ݵĴ�С����������ͷ��С
	_packethead()
	{
		memset(this, 0, sizeof(*this));
	}
}PACKETHEAD;
#pragma pack(pop)

//���ݱ�������
typedef struct _packetdata
{
	PACKETHEAD head;
	char *data; //����������ݵ���ʼ��ַ
	unsigned short recvlen; //���յ��Ĵ�С
	char buffer[MAX_BUFFER_LEN];//������

	_packetdata()
	{
		memset(this, 0, sizeof(*this));
	}
}PACKETDATA,*LPPACKETDATA;


class CBaseServer
{
	typedef std::map<evutil_socket_t, LPPACKETDATA> BUFFERLIST;
	typedef std::pair<evutil_socket_t, LPPACKETDATA> BUFFERITEM;
public:
	CBaseServer();
	~CBaseServer();

	//��������
	static void SendData(bufferevent *bev, char *data, unsigned short len);
	
	//�ر�����
	void Close(bufferevent *bev);

	//��ʼ��
	void Initialize();
	//���÷�������Ķ˿�
	void SetPort(USHORT port);
	//��ʼ���� isBlock�Ƿ�����
	int Start(bool isBlock);
	//�رշ���
	void Stop();

	//�Ƿ��ʼ��
	bool isInit() { return m_isInit; }
	//�Ƿ�����
	bool isStart() { return m_isStart; }
protected:
	//����ֻ��Ҫ���������¼�����
	//���ӹر� bev:�¼����
	virtual void OnClose(bufferevent *bev);
	//�������� bev:�¼����, ip:socket��ip��ַ
	virtual void OnConnect(bufferevent *bev, char *ip);
	//����һ����Ϣ
	virtual void OnProcessMessage(bufferevent *bev, char *message, unsigned short len);

private:
	//�߳�ִ���¼�ѭ��
	static void MultEventLoop(event_base* base = nullptr);
	//�յ�����
	static void ReadEvent(bufferevent *bev, void *arg);
	//�������
	static void WriteEvent(bufferevent *bev, void *arg);
	//�������� arg��BaseServer��ʵ��ָ��
	static void AcceptEvent(evconnlistener* listener, evutil_socket_t fd, sockaddr *sock, int socklen, void *arg);
	//������
	static void ErrorEvent(bufferevent *bev, short events, void *arg);
	//�����ź��¼�
	static void SignalEvent(evutil_socket_t sig, short events, void *arg);


	//��ȡ������ ����true��ʾ��ȡ����������
	static bool GetPacketData(PACKETDATA &data, evbuffer* buffer);
private:
	event_base *m_base;
	evconnlistener *m_listener;

	USHORT  m_port;
	WSADATA m_data;
	sockaddr_in m_addr;

	BUFFERLIST m_bufferlist; //���ݻ����б�
	std::mutex m_bufferlistmutex; //���ݻ����б����

	bool m_isInit;//�Ƿ��ʼ��
	bool m_isStart;//�Ƿ�����

	static int m_isInitNetLib;
};

