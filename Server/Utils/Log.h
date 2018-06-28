#ifndef __LOG_H__
#define	__LOG_H__
/**************************************************************************************
 * FILE:log.h
 * DATE:2017/12/11
 * AUTH:YangW
 * INTR:Log基类
**************************************************************************************/

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <execinfo.h>
#include <stdlib.h>
#include "../PublicCommon.h"
#include "../Module/ModuleLock.h"
US_NS_MODULE;


#define MSG_LENGTH		1024 * 5


enum LEVEL
{
	Info   = 0x01,
	Notice = 0x02,
	Debug  = 0x03,
	Warn   = 0x04,
	Error  = 0x05
};

class Log
{
	~Log();
private:
	Log();
	static Log* instance;
	AALock* m_logLock;
	//pthread_mutex_t* _mutex;
	char* message;
	
private:
	void Log_Message(LEVEL level);

	SYNTHESIZE(char, lv, LogLevel);
public:
	static Log* getInstance();
	
	static void Debug(const char* format,...);
	static void Info(const char* format,...);
	static void Warn(const char* format,...);
	static void Error(const char* format,...);
	static void Notice(const char* format, ...);
};


#endif //__LOG_H__