#include "stdafx.h"
#include "BaseServer.h"

int CBaseServer::m_isInitNetLib = 0;

CBaseServer::CBaseServer()
{
	if (m_isInitNetLib <= 0)
	{
		WSAStartup(MAKEWORD(2, 2), &m_data);
	}
	m_isInitNetLib++;

	m_base = nullptr;
	m_listener = nullptr;
	m_isInit = false;
	m_isStart = false;
	m_base = nullptr;
	m_port = 0;
	memset(&m_addr, 0, sizeof(m_addr));
}

CBaseServer::~CBaseServer()
{
	if (m_isStart && m_listener)
	{
		evconnlistener_free(m_listener);
	}

	if (m_isInit && m_base)
	{
		event_base_free(m_base);
	}

	m_isInitNetLib--;
	if (m_isInitNetLib <= 0)
	{
		WSACleanup();
	}
}

void CBaseServer::SignalEvent(evutil_socket_t sig, short events, void *arg)
{
	event_base_loopexit((event_base*)arg, nullptr);
	printf("^C");
}

void CBaseServer::AcceptEvent(evconnlistener* listener, evutil_socket_t fd, sockaddr *sock, int socklen, void *arg)
{
	if (arg == nullptr) return;

	sockaddr_in *sock_in = (sockaddr_in*)sock;

	char ip[20] = { 0 };
	evutil_inet_ntop(sock_in->sin_family, &sock_in->sin_addr, ip, sizeof(ip));

	printf("There's a new connection, fd:%d,ip address:%s\n", (int)fd, ip);

	event_base * base = evconnlistener_get_base(listener);

	bufferevent *evt = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
	if (!evt) return;

	if (evutil_make_socket_nonblocking(fd) == -1)	//设置套接字为非阻塞
	{
		bufferevent_free(evt);
		return;
	}

	//给连接创建缓冲区
	evutil_socket_t sockid = bufferevent_getfd(evt);
	if (sockid == -1)
	{
		bufferevent_free(evt);
		return;
	}
	
	CBaseServer *BaseServer = (CBaseServer*)arg;

	LPPACKETDATA buffer = new PACKETDATA; //申请内存缓冲区

	BaseServer->m_bufferlistmutex.lock();
	BaseServer->m_bufferlist.insert(BUFFERITEM(sockid, buffer));
	//BaseServer->m_bufferlist[sockid] = buffer;
	BaseServer->m_bufferlistmutex.unlock();

	bufferevent_set_max_single_read(evt, MAX_BUFFER_LEN); //设置单次回调最大读取字节数

	bufferevent_setcb(evt, ReadEvent, WriteEvent, ErrorEvent, arg);
	if (bufferevent_enable(evt, EV_READ | EV_WRITE) == -1)
	{
		delete buffer;
		bufferevent_free(evt);
		return;
	}

	BaseServer->OnConnect(evt, ip);
}

void CBaseServer::ReadEvent(bufferevent *bev, void *arg)
{
	if (arg == nullptr) return;

	evutil_socket_t sockid = bufferevent_getfd(bev);
	if (sockid == -1) return;

	CBaseServer *BaseServer = (CBaseServer*)arg;

	BaseServer->m_bufferlistmutex.lock(); //列表锁

	BUFFERLIST::iterator iter = BaseServer->m_bufferlist.find(sockid);
	
	if (iter == BaseServer->m_bufferlist.end())
	{
		BaseServer->m_bufferlistmutex.unlock();
		return;
	}

	PACKETDATA data = *(iter->second);

	BaseServer->m_bufferlistmutex.unlock();

	evbuffer* input_evbuffer = bufferevent_get_input(bev);
	
	while (GetPacketData(data, input_evbuffer))
	{
		BaseServer->OnProcessMessage(bev, data.data, data.head.size);
		memset(&data, 0, sizeof(data));
	}


	BaseServer->m_bufferlistmutex.lock();
	
	BUFFERLIST::iterator iter1 = BaseServer->m_bufferlist.find(sockid);

	if (iter1 == BaseServer->m_bufferlist.end())
	{
		BaseServer->m_bufferlistmutex.unlock();
		return;
	}

	memcpy(iter1->second, &data, sizeof(data));
	BaseServer->m_bufferlistmutex.unlock();
}

void CBaseServer::WriteEvent(bufferevent *bev, void *arg)
{

}

void CBaseServer::ErrorEvent(bufferevent *bev, short events, void *arg)
{
	if (arg == nullptr) return;
	CBaseServer *BaseServer = (CBaseServer*)arg;

	if (BEV_EVENT_ERROR & events)//遇到错误
	{
		puts("BEV_EVENT_ERROR");
		BaseServer->Close(bev); 
		//bufferevent_free(bev);
	}
	else if (BEV_EVENT_EOF & events)//连接关闭
	{
		puts("Connect the Close.");
		BaseServer->Close(bev);
		//bufferevent_free(bev);
	}
	else if (BEV_EVENT_TIMEOUT & events)//超时
	{
		puts("BEV_EVENT_TIMEOUT");
		BaseServer->Close(bev);
		//bufferevent_free(bev);
	}

//暂时没用到
//	if (BEV_EVENT_READING & events)//读的时候遇到错误
//		puts("BEV_EVENT_READING");

//	if (BEV_EVENT_WRITING & events)//写的时候遇到错误
//		puts("BEV_EVENT_WRITING");
}
void CBaseServer::Close(bufferevent *bev)
{
	OnClose(bev);

	evutil_socket_t sockid = bufferevent_getfd(bev);
	if (sockid == -1) return;

	bufferevent_disable(bev, EV_READ | EV_WRITE);
	bufferevent_free(bev);

	m_bufferlistmutex.lock();

	BUFFERLIST::iterator iter = m_bufferlist.find(sockid);

	if (iter == m_bufferlist.end())
	{
		m_bufferlistmutex.unlock();
		return;
	}

	if (iter->second != nullptr)
	{
		delete (iter->second);//删除缓冲区
		iter->second = nullptr;
	}

	m_bufferlist.erase(iter);//从map中删除

	m_bufferlistmutex.unlock();
}

void CBaseServer::OnClose(bufferevent *bev)
{
}

void CBaseServer::OnConnect(bufferevent *bev, char *ip)
{
}

void CBaseServer::OnProcessMessage(bufferevent *bev, char *message, unsigned short len)
{
}

void CBaseServer::Initialize()
{
	evthread_use_windows_threads();//使用多线程

	struct event_config* cfg = event_config_new();
	event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);
	
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	event_config_set_num_cpus_hint(cfg, si.dwNumberOfProcessors);//根据CPU实际数量配置libEvent的CPU数

	m_base = event_base_new_with_config(cfg);
	event_config_free(cfg);

	if (m_base == nullptr)
	{
		m_isInit = false;
		return;
	}

	event *ev = evsignal_new(m_base, SIGINT, SignalEvent, m_base);

	m_isInit = true;
}

void CBaseServer::SetPort(USHORT port)
{
	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	m_addr.sin_port = htons(port);
	m_port = port;
}

int CBaseServer::Start(bool isBlock)
{
	if (!m_isInit) return -1;
	if (m_addr.sin_port == 0) return -2;
	if (m_isStart) return -3;

	m_listener = evconnlistener_new_bind(m_base, AcceptEvent, this, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
		(sockaddr *)&m_addr, sizeof(m_addr));

	//开始阻塞事件,这里可以在多线程里阻塞，这样主线程就不会被阻塞住
	if (isBlock)
	{
		event_base_dispatch(m_base);
	}
	else
	{
		std::thread ht(MultEventLoop, m_base);
		ht.detach();
	}
	m_isStart = true;
	return 0;
}

void CBaseServer::MultEventLoop(event_base* base)
{
	if (base == nullptr) return;
	event_base_dispatch(base);
}

void CBaseServer::Stop()
{
	if (m_base == nullptr || m_listener == nullptr || m_isInit == false || m_isStart == false) return;

	event_base_loopexit(m_base, nullptr);
	evconnlistener_free(m_listener);

	m_listener = nullptr;
	m_isStart = false;
}

void CBaseServer::SendData(bufferevent *bev, char *data, unsigned short len)
{
	bufferevent_write(bev, &len, sizeof(len));
	bufferevent_write(bev, data, len);
}

bool CBaseServer::GetPacketData(PACKETDATA &data, evbuffer* buffer)
{
	unsigned short nBufferSize = (unsigned short)evbuffer_get_length(buffer);
	if (nBufferSize <= 0) return false;

	// recv packet head
	if (data.recvlen < PACKET_HEAD_SIZE)
	{
		if (data.recvlen + nBufferSize < PACKET_HEAD_SIZE)//已接收+可接收不够一个包头
		{
			evbuffer_remove(buffer, data.buffer + data.recvlen, nBufferSize);
			data.recvlen += nBufferSize;
			return false;
		}
		else
		{
			evbuffer_remove(buffer, data.buffer + data.recvlen, PACKET_HEAD_SIZE - data.recvlen);
			nBufferSize -= (PACKET_HEAD_SIZE - data.recvlen);
			data.recvlen += (PACKET_HEAD_SIZE - data.recvlen);
			memcpy(&data.head, data.buffer, sizeof(data.head));
			if (nBufferSize <= 0)
				return false;
		}
	}

	if (data.head.size > MAX_BUFFER_LEN)
	{
		memset(&data, 0, sizeof(data));
		return false;
	}

	//if head
	//recv packet
	unsigned short remainCount = data.recvlen - PACKET_HEAD_SIZE;//已经接收的包体
	if (remainCount + nBufferSize < data.head.size)//不够一个数据包
	{
		evbuffer_remove(buffer, data.buffer + data.recvlen, nBufferSize);
		data.recvlen += nBufferSize;
		return false;
	}
	else
	{
		evbuffer_remove(buffer, data.buffer + data.recvlen, data.head.size - remainCount);
		data.recvlen += (data.head.size - remainCount);
		data.data = data.buffer + PACKET_HEAD_SIZE;
	}

	return true;
}