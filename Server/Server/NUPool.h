#ifndef __NUPOOL_H__
#define __NUPOOL_H__
/**************************************************************************************
 * FILE:nupool.h
 * DATE:2017/12/20
 * AUTH:YangW
 * INTR:网络线程池实例，服务器线程池处理
**************************************************************************************/

#include "../Module/ModuleThreadPool.h"
#include "../Utils/Log.h"
#include <sys/epoll.h>
#include "../PublicCommon.h"
#include "./NUSocket.h"

US_NS_MODULE;

class GameServer;


void TreadProc(const char* data, void* pthread, void* target);

class NUPool :public ModuleThreadPool
{
public:
	~NUPool();
	NUPool(const NUPool&) = delete;
	NUPool(int threadNum, const char* data, void* target);

	virtual bool init();
	virtual bool run(bool tag = true);
	virtual bool stop();

	//友元
	friend void TreadProc(const char* data, void* pthread, void* target);
private:
	//struct epoll_event ev;

	AALock* g_threadLock;

	GameServer* g_server;
protected:
	//设置非阻塞
	void setnonblocking(SOCKET sock);
	
	//建立新的连接
	bool setnewconnect(SOCKET listenFD,int epfd, _msgData* msg, char* data);

	//读取数据
	bool readSocketMsg(SOCKET fd,_msgData* msg,char* data);

	//发送数据
	bool sendSocketMsg(SOCKET fd);
};

#endif // !__NUPOOL_H__

