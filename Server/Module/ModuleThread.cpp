#include "ModuleThread.h"
#include <string.h>
#include "../Utils/Log.h"

NS_MODULE_BEGINE

ModuleThread::ModuleThread(const char * data, Task proc, void * target)
	:Module(),m_data(nullptr),m_treadProc(nullptr),m_target(nullptr)
{
	if (data)
	{
		int len = strlen(data) + 1;
		m_data = new char[len];
		strcpy(m_data, data);
	}
	if (proc)
		m_treadProc = proc;
	if (target)
		m_target = target;
}

ModuleThread::~ModuleThread()
{
	this->setRunFlag(false);
	this->setTag(0);
	SAFE_DELETE_ARRAY(m_data);
	SAFE_DELETE(this->m_workThread);
}

ModuleThread * ModuleThread::create(const char * data, Task proc, void * target)
{
	ModuleThread* pRet = new ModuleThread(data, proc, target);
	if (!pRet || !pRet->init())
		SAFE_DELETE(pRet);
	return pRet;
}

bool ModuleThread::init()
{
	if (!Module::init()) {
		return false;
	}

	if (this->m_treadProc == nullptr) {
		return false;
	};

	return true;
}

bool ModuleThread::run(bool detach)
{
	if (!Module::run()) {
		return false;
	}

	this->m_workThread = new std::thread(this->m_treadProc, m_data, this, m_target);
	Log::Info("thread [%d] is running.", this->m_workThread->get_id());
	if (detach)
		this->m_workThread->detach();
	else
		this->m_workThread->join();


	return true;
}

bool ModuleThread::stop()
{
	if (!Module::stop()) {
		return false;
	}

	delete this;
	return true;
}


NS_MODULE_END
