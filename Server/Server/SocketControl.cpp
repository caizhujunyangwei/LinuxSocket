#include "SocketControl.h"
#include "../Utils/Log.h"
#include "../PublicCommon.h"

SocketControl* SocketControl::instance = nullptr;

SocketControl::SocketControl()
{
	m_sokcetsLock = nullptr;
	m_deleteLock = nullptr;
}

SocketControl::~SocketControl()
{
	deleteAllSockets();

	ModuleLock::clearLock(m_sokcetsLock);
	ModuleLock::clearLock(m_deleteLock);
}

SocketControl * SocketControl::getInstance()
{
	if (!instance) {
		instance = new SocketControl();
		if (!instance || !instance->init()) {
			SAFE_DELETE(instance);
		}
	}

	return instance;
}

bool SocketControl::init()
{
	m_sokcetsLock = ModuleLock::createAALock(Lock_AllSockets, SPIN_LOCK);
	if (!m_sokcetsLock) {
		return false;
	}
	m_deleteLock = ModuleLock::createAALock(Lock_DeleteSockets, SPIN_LOCK);
	if (!m_deleteLock) {
		return false;
	}

	return true;
}

int SocketControl::pushNewSocket(SOCKET st, NUSocket * ns)
{
	if (this->m_sockets.size() >= MaxRecvNum) {
		Log::Error("SOCKETS recv is get maxNum!");
		return 2;
	}

	if (this->getNUSocket(st)) {
		Log::Error("A same SOCKET is already connected!");
		return 1;
	}

	//推入玩家
	ModuleLock::setSpinLock(m_sokcetsLock, true);
	this->m_sockets[st] = ns;
	ModuleLock::setSpinLock(m_sokcetsLock, false);

	return 0;
}

bool SocketControl::deleteSocket(SOCKET st)
{
	if (!this->m_sockets[st]) {
		return false;
	}
	NUSocket* ns = this->m_sockets[st];
	auto it = this->m_sockets.find(st);
	if (this->m_sockets.end() != it) {
		Log::Error("find socket error happened!");
		return false;
	}
	ModuleLock::setSpinLock(m_sokcetsLock, true);
	this->m_sockets.erase(it);
	ModuleLock::setSpinLock(m_sokcetsLock, false);

	//推入玩家
	ModuleLock::setSpinLock(m_deleteLock, true);
	this->m_deleteList.push_back(ns);
	ModuleLock::setSpinLock(m_deleteLock, false);


	return true;
}

bool SocketControl::deleteSocket(NUSocket * ns)
{
	if (!this->m_sockets[ns->getFD()]) {
		return false;
	}
	auto it = this->m_sockets.find(ns->getFD());
	if (it!= this->m_sockets.end()) {
		Log::Error("find socket error happened!"); 
		return false;
	}
	ModuleLock::setSpinLock(m_sokcetsLock, true);
	this->m_sockets.erase(it);
	ModuleLock::setSpinLock(m_sokcetsLock, false);

	//推入玩家
	ModuleLock::setSpinLock(m_deleteLock, true);
	this->m_deleteList.push_back(ns);
	ModuleLock::setSpinLock(m_deleteLock, false);

	return true;
}

bool SocketControl::deleteAllSockets()
{
	ModuleLock::setSpinLock(m_deleteLock, true);
	for (int i = 0; i < this->m_sockets.size(); ++i)
	{
		//推入玩家
		this->m_deleteList.push_back(this->m_sockets[i]);
	}
	ModuleLock::setSpinLock(m_deleteLock, false);

	//推入玩家
	ModuleLock::setSpinLock(m_sokcetsLock, true);
	this->m_sockets.clear();
	ModuleLock::setSpinLock(m_sokcetsLock, false);

	this->dealDeleteSockets();

	return false;
}

bool SocketControl::dealDeleteSockets()
{
	NUSocket* pRet = nullptr;
	for (size_t i = 0; i < this->m_deleteList.size(); i++)
	{
		pRet = this->m_deleteList.back();
		(pRet->m_userLock->m_spinMutex->lock());
		{
			close(pRet->getFD());
			SAFE_DELETE(pRet);
		}
	}

	//推入玩家
	if (this->m_deleteList.size() > 0) {
		ModuleLock::setSpinLock(m_deleteLock, true);
		this->m_deleteList.clear();
		ModuleLock::setSpinLock(m_deleteLock, false);
	}

	return true;
}

NUSocket * SocketControl::getNUSocket(SOCKET st)
{
	return this->m_sockets[st];
}

