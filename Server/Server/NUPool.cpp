#include "NUPool.h"
#include "./GameServer.h"
#include <sstream>
#include "../Utils/ThirdParty/SHA1.h"
#include "../Utils/ThirdParty/Base64.h"
#include "../Utils/ThirdParty/MD5.h"
#include <assert.h>

NUPool::NUPool(int threadNum, const char * data, void * target)
	:ModuleThreadPool(threadNum,data,TreadProc,target)
{
	g_server = (GameServer*)target;
}

NUPool::~NUPool()
{
	ModuleLock::clearLock(g_threadLock);
}

bool NUPool::init()
{
	if (!ModuleThreadPool::init()) {
		return false;
	}

	//初始化线程池相关
	g_threadLock = ModuleLock::createAALock(Lock_NUPool, MUTEX_LOCK);

	Log::Info("初始化服务器线程池完成");

	return true;
}

bool NUPool::run(bool tag)
{
	Log::Info("开始服务器线程池");
	if (!ModuleThreadPool::run(true)) {
		return false;
	}

	return true;
}

bool NUPool::stop()
{
	if (!ModuleThreadPool::stop()) {
		return false;
	}


	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NUPool::setnonblocking(SOCKET sock)
{
	int opts;
	opts = fcntl(sock, F_GETFL);
	if (opts < 0)
	{
		Log::Error("fcntl(sock,GETFL)");
		return;
	}
	opts = opts | O_NONBLOCK;
	if (fcntl(sock, F_SETFL, opts) < 0)
	{
		Log::Error("fcntl(sock,SETFL,opts)");
		return;
	}
}

bool NUPool::setnewconnect(SOCKET listenFD, int epfd, _msgData* msg, char* data)
{
	struct sockaddr_in clientaddr = { 0 };
	socklen_t clilen = sizeof(clientaddr);
	SOCKET fd = accept(listenFD, (sockaddr*)&clientaddr, &clilen);
	if (fd == -1) {
		Log::Error("accept socket failed!!!");
		return false;
	}
	setnonblocking(fd);

	//int nRecvBuf = 128 * 1024;//设置为32K  
	//setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
	////发送缓冲区  
	//int nSendBuf = 128 * 1024;//设置为32K  
	//setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int));
	//int nNetTimeout = 1000;//1秒  
	////发送时限  
	//setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&nNetTimeout, sizeof(int));
	////接收时限  
	//setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&nNetTimeout, sizeof(int));

	NUSocket* pRet = NUSocket::create(fd, clientaddr);

	struct epoll_event ev;
	ev.data.fd = fd;
	//设置用于注测的读操作事件
	ev.events = EPOLLIN | EPOLLET;
	//ev.events=EPOLLIN;
	//注册ev
	epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

	return true;
}

bool NUPool::readSocketMsg(SOCKET fd,_msgData* msg, char* data)
{
	msg->_msgLength = 0;
	msg->_type = 0x00;

	int recvNum = read(fd, data, MAX_RECV_SIZE);

	if (recvNum < 0) //出错了啊
	{
		Log::Error("recv msg error,so drop this msg");
		if (errno == EAGAIN)
		{
			// 由于是非阻塞的模式,所以当errno为EAGAIN时,表示当前缓冲区已无数据可读
			// 在这里就当作是该次事件已处理.
			Log::Error("read EAGAIN");
			return false;
		}
		else if (errno == ECONNRESET)
		{
			// 对方发送了RST
			//shutdown(fd, SHUT_RD);
			Log::Warn("socket send close");
		}
		else if (errno == EINTR)
		{
			// 被信号中断
			Log::Error("被信号中断");
			return false;
		}
		else
		{
			//其他不可弥补的错误
			//shutdown(fd, SHUT_RD);
			Log::Error("something happened");
			return false;
		}
	}
	else if (recvNum == 0)
	{
		// 这里表示对端的socket已正常关闭.发送过FIN了。
		Log::Warn("socket close success");
	}
	else {
		
		//重置接收缓冲区
		//memset(data, 0, MAX_RECV_SIZE);
	}

	msg->_type &= 0; //设置消息不可再用
	return true;
}

bool NUPool::sendSocketMsg(SOCKET fd)
{
	//暂时没有处理
	return false;
}


//服务器处理线程 多线程处理
//@data ：线程输入内容
//@pthread ：线程
//@target ：启动TreadProc的实例
void TreadProc(const char * data, void * _pthread, void * target)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(100)); //确保线程运行
	Log::Debug("进入线程池，%s", data);

	
	ModuleThread* pthread = (ModuleThread*)_pthread;
	GameServer* server = (GameServer*)target;
	NUPool* pool = server->getNUPoll();

	struct epoll_event events[MAX_EPOLL_NUM];
	int epfd = server->getEpfd(); //服务器ev事件
	SOCKET listenFD = server->getListenFD();

	//assert(!epfd || !pool || !listenFD);

	bool needMutex = pool->getThreadNum() == 1;

	//格式化收到的消息
	_msgData* msgData = new _msgData();
	//临时存储消息的
	char* tempData = new char[MAX_RECV_SIZE];

	int i = 0;
	int size = 0;
	while (pthread->getRunFlag())
	{
		if (needMutex) {
			pool->g_threadLock->m_mutex->lock();
		}
		size = 0;
		size = epoll_wait(epfd, events, MAX_EPOLL_NUM,-1);
		Log::Info("recv epool events ===================:%d",size);
		if (needMutex) {
			pool->g_threadLock->m_mutex->unlock();
		}

		//循环取出所有的events处理
		for (i=0;i < size;++i)
		{
			//如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接。
			if (events[i].data.fd == listenFD)
			{
				if (!pool->setnewconnect(listenFD, epfd, msgData,tempData))
				{
					Log::Error("new connect socket failed");
				}
			}
			//如果是已经连接的用户
			else
			{
				if (events[i].events & EPOLLIN) //收到数据，那么进行读入
				{
					if (events[i].data.fd < 0)
					{
						Log::Error("recv data socket error");
						continue;
					}
					if (!pool->readSocketMsg(events[i].data.fd, msgData, tempData))
					{
						Log::Error("read msg failed");
					}
					//接收消息+1 可能会不准，多线程不安全
					server->setRecvNum(server->getRecvNum() + 1);
				}
				//if (events[i].events & EPOLLOUT)	// 如果有数据发送 暂时没有用的到
				//{
				//	if (events[i].data.fd < 0)
				//	{
				//		Log::Error("send data socket error");
				//		continue;
				//	}
				//	if (!pool->sendSocketMsg(events[i].data.fd))
				//	{
				//		Log::Error("send msg failed");
				//	}
				//}
			}
		}

		memset(tempData, 0, MAX_RECV_SIZE);
	}

	Log::Debug("退出线程池，%s", data);
	SAFE_DELETE_ARRAY(tempData);
	SAFE_DELETE(msgData);
}
