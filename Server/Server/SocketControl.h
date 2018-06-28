#ifndef __SOCKET_CONTROL_H__
#define __SOCKET_CONTROL_H__
/**************************************************************************************
* FILE:nupool.h
* DATE:2017/12/20
* AUTH:YangW
* INTR:链接列表控制器
**************************************************************************************/
#include "./NUSocket.h"

class SocketControl
{
private:
	SocketControl();
	
	static SocketControl* instance;

protected:
	virtual bool init();

	std::map<SOCKET, NUSocket*> m_sockets;

	std::list<NUSocket*> m_deleteList; //交由主线程来移除

	AALock* m_sokcetsLock; //m_sockets控制锁
	AALock* m_deleteLock;//m_deleteList	控制锁

public:
	virtual ~SocketControl();
	static SocketControl* getInstance();

public:
	//加入新的玩家
	//0 成功
	//1 已经有该socket了
	//2 玩家列表达上线了
	//3 其他错误
	int pushNewSocket(SOCKET st, NUSocket* ns);

	//删除玩家
	bool deleteSocket(SOCKET st);
	bool deleteSocket(NUSocket* ns);
	bool deleteAllSockets();

	//清理删除表中的玩家
	bool dealDeleteSockets();

	//获取玩家信息
	NUSocket* getNUSocket(SOCKET st);

public:
	//获取所有玩家数量
	unsigned int getAllSocketsSize();
};



inline unsigned int SocketControl::getAllSocketsSize()
{
	return this->m_sockets.size();
}


#endif // !__SOCKET_CONTROL_H__

