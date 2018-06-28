#ifndef __NUSOCKET_H__
#define __NUSOCKET_H__
/**************************************************************************************
 * FILE:nusocket.h
 * DATE:2017/12/11
 * AUTH:YangW
 * INTR:远端套接字封装类
**************************************************************************************/
#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <vector>
#include <map>
#include <list>
#include "../PublicCommon.h"
#include "../Utils/Log.h"
#include "../Module/ModuleLock.h"
#include "../Utils/ThirdParty/tinyxml2.h"
#include "../Module/ModuleTime.h"
US_NS_MODULE;

//消息结构体
class _msgData
{
public:
	//消息状态 
	//0无用
	//1可用
	char _type;	
	//消息长度
	unsigned int _msgLength;
	//消息
	char* _data;

	//构造消息
	_msgData() {
		this->_type = 0;
		this->_msgLength = 0;
		this->_data = new char [MAX_RECV_SIZE];
	}

	//清理发送了的消息
	~_msgData() {
		this->_type = 0;
		SAFE_DELETE_ARRAY(_data);
	}
};

class NUSocket
{
	NUSocket(SOCKET sock, sockaddr_in& addr);
public:
	friend class GameServer;

	virtual ~NUSocket();
	static NUSocket* create(SOCKET, sockaddr_in&);
protected:
	virtual bool init();

	SYNTHESIZE(SOCKET_TYPE, sockettype, SocketType);		//套接字类型
	SYNTHESIZE(SOCKET_STATE, socketstate, SocketState);		//套接字状态
	SYNTHESIZE(SOCKET, fd, FD);								//套接字
	SYNTHESIZE(char*, ip, IP);								//Ip地址
	SYNTHESIZE(sockaddr_in*, sockaddr, SockAddr);			//套接字地址详情
	SYNTHESIZE(time_t, lastHeart, LastHeartTime);			//记录上一次的心跳时间
	
public:
	moduleTask::AALock* m_userLock;	//玩家操作锁

};


#endif //!__NUSOCKET_H__
