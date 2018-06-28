#ifndef __GAMESERVER_H__
#define __GAMESERVER_H__
/**************************************************************************************
 * FILE:gameserver.h
 * DATE:2017/12/11
 * AUTH:YangW
 * INTR:服务器基本类 
**************************************************************************************/

#include "./NUSocket.h"
#include "./NUPool.h"
US_NS_MODULE;


class GameServer
{
private:
	GameServer(unsigned short _port, unsigned int max);

	virtual bool init();

public:
	virtual ~GameServer();
	//监听端口号和最大连接人数
	static GameServer* create(unsigned short _port, unsigned int max);
	
	SYNTHESIZE(unsigned short, port, Port);				//监听端口号
	SYNTHESIZE(unsigned int, maxrecv, MaxRecvNum);			//最大接收数
	SYNTHESIZE(SOCKET, fd, ListenFD);					//监听套接字
	SYNTHESIZE(sockaddr_in*, serveraddr, ServerAddr);	//服务器地址信息
	SYNTHESIZE(unsigned int, sendnum, SendNum);					//发送的消息数量
	SYNTHESIZE(unsigned int, recvnum, RecvNum);					//接收的消息数量
	SYNTHESIZE(int, epfd, Epfd);
	
	//运行标志
	SYNTHESIZE(bool, isRun, IsRun);

	SYNTHESIZE(NUPool*, m_nupool, NUPoll);

public:
	virtual bool stop();
	virtual bool run();

	//发送消息
	bool sendSocketMsg(SOCKET socket,unsigned char* msg, unsigned int len, bool needClean = true);

};

#endif // !__GAMESERVER_H__
