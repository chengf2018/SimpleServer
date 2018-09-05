#pragma once
/*
	BaseServer 基础服务框架
	使用时只需要创建一个类继承CBaseServer，并继承OnClose，OnConnect，OnProcessMessage类方法即可
	
	使用示例：
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



#define MAX_BUFFER_LEN 65535 //数据报的最大长度
#define PACKET_HEAD_SIZE sizeof(PACKETHEAD) //包头大小

//包头定义，目前只有一个包大小，可以扩展
#pragma pack(push,1)
typedef struct _packethead
{
	unsigned short size; //数据的大小，不包含包头大小
	_packethead()
	{
		memset(this, 0, sizeof(*this));
	}
}PACKETHEAD;
#pragma pack(pop)

//数据报缓冲区
typedef struct _packetdata
{
	PACKETHEAD head;
	char *data; //存放真正数据的起始地址
	unsigned short recvlen; //接收到的大小
	char buffer[MAX_BUFFER_LEN];//缓冲区

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

	//发送数据
	static void SendData(bufferevent *bev, char *data, unsigned short len);
	
	//关闭连接
	void Close(bufferevent *bev);

	//初始化
	void Initialize();
	//设置服务监听的端口
	void SetPort(USHORT port);
	//开始监听 isBlock是否阻塞
	int Start(bool isBlock);
	//关闭服务
	void Stop();

	//是否初始化
	bool isInit() { return m_isInit; }
	//是否启动
	bool isStart() { return m_isStart; }
protected:
	//子类只需要覆盖以下事件即可
	//连接关闭 bev:事件句柄
	virtual void OnClose(bufferevent *bev);
	//连接上线 bev:事件句柄, ip:socket的ip地址
	virtual void OnConnect(bufferevent *bev, char *ip);
	//处理一条消息
	virtual void OnProcessMessage(bufferevent *bev, char *message, unsigned short len);

private:
	//线程执行事件循环
	static void MultEventLoop(event_base* base = nullptr);
	//收到数据
	static void ReadEvent(bufferevent *bev, void *arg);
	//发送完成
	static void WriteEvent(bufferevent *bev, void *arg);
	//连接上线 arg是BaseServer的实例指针
	static void AcceptEvent(evconnlistener* listener, evutil_socket_t fd, sockaddr *sock, int socklen, void *arg);
	//错误处理
	static void ErrorEvent(bufferevent *bev, short events, void *arg);
	//触发信号事件
	static void SignalEvent(evutil_socket_t sig, short events, void *arg);


	//获取包数据 返回true表示获取完整包数据
	static bool GetPacketData(PACKETDATA &data, evbuffer* buffer);
private:
	event_base *m_base;
	evconnlistener *m_listener;

	USHORT  m_port;
	WSADATA m_data;
	sockaddr_in m_addr;

	BUFFERLIST m_bufferlist; //数据缓冲列表
	std::mutex m_bufferlistmutex; //数据缓冲列表的锁

	bool m_isInit;//是否初始化
	bool m_isStart;//是否启动

	static int m_isInitNetLib;
};

