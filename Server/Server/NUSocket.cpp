#include "NUSocket.h"


NUSocket::NUSocket(SOCKET sock, sockaddr_in& addr)
	:fd(-1)
{
	this->setFD(sock);
	this->sockaddr = new sockaddr_in();
	memcpy(this->sockaddr, &addr, sizeof(sockaddr));
	setSocketState(SOCKET_STATE::SOCKET_STATE_NULL);
	setSocketType(SOCKET_TYPE::SOCKET_TYPE_NULL);
}

NUSocket::~NUSocket()
{
	SAFE_DELETE(this->sockaddr);
	SAFE_DELETE_ARRAY(this->ip);
	setFD(-1);
	ModuleLock::clearLock(m_userLock);
	setSocketState(SOCKET_STATE::SOCKET_STATE_NULL);
	setSocketType(SOCKET_TYPE::SOCKET_TYPE_NULL);
}

NUSocket * NUSocket::create(SOCKET s, sockaddr_in & addr)
{
	NUSocket* pRet = new NUSocket(s, addr);
	if (!pRet || !pRet->init())
	{
		SAFE_DELETE(pRet);
	}
	return pRet;
}

bool NUSocket::init()
{
	if(!this->getFD())
		return false;

	//创建自旋锁
	m_userLock = ModuleLock::createAALock(getFD(), SPIN_LOCK);

	char* tips = inet_ntoa(sockaddr->sin_addr);
	int len = strlen(tips) + 1;
	char* tip = new char[len];
	strcpy(tip,tips);
	Log::Info("A new client connect from: %s\n", tip);
	setIP(tip);

	//初始化心跳时间
	setLastHeartTime(ModuleTime::getLocalTimeNumber());


	return true;
}

