#include "Log.h"
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "../Module/ModuleTime.h"

Log* Log::instance = NULL;

Log::~Log()
{
	instance = nullptr;
	SAFE_DELETE_ARRAY(message);
	ModuleLock::clearLock(this->m_logLock);
}

Log::Log()
{
	lv = LEVEL::Info;
	message = new char[MSG_LENGTH];
	memset(this->message, 0, MSG_LENGTH);
	m_logLock = ModuleLock::createAALock(Log_Lock, SPIN_LOCK);
}

void Log::Log_Message(LEVEL level)
{
	static char lv[16] = {};
	static char color[16]={};
	static struct tm tt;

	switch (level)
	{
		case LEVEL::Debug:
		{
			strcpy(lv, "DEBUG");
			strcpy(color, "\033[32m");
		}
		break;
		case LEVEL::Info:
		{
			strcpy(lv, "INFO");
			strcpy(color, "\033[37m");
		}
		break;
		case LEVEL::Warn:
		{
			strcpy(lv, "Warn");
			strcpy(color, "\033[33m");
		}
		break;
		case LEVEL::Error:
		{
			strcpy(lv, "ERROR");
			strcpy(color, "\033[31m");
		}
		break;
		case LEVEL::Notice:
		{
			strcpy(lv, "Notice");
			strcpy(color, "\033[36m");
		}
		break;
		default:
		{
			strcpy(lv, "UNKNOWN");
			strcpy(color, "\033[35m");
		}
		break;
	}
	
	tt = ModuleTime::getLocalTime();
	//printf("%s ", moduleTask::ModuleTime::convert2ASCTime(tt));
	printf("%s%d-%d-%d %d:%0.2d:%0.2d [%s] %s\n\033[0m", color, 
		1900+tt.tm_year,1+tt.tm_mon,tt.tm_mday,tt.tm_hour,tt.tm_min,tt.tm_sec,
		lv, message);

	fflush(stdout);
}

Log * Log::getInstance()
{
	if (!instance)
		instance = new Log();

	return instance;
}

void Log::Debug(const char* format, ...)
{
	if (!instance)
		instance = new Log();
	if (instance->getLogLevel() > LEVEL::Debug) {
		return;
	}

	ModuleLock::setSpinLock(instance->m_logLock, true);
	//memset(instance->message, 0, MSG_LENGTH);

	va_list argptr;
	va_start(argptr, format);
	vsnprintf(instance->message, MSG_LENGTH, format, argptr);
	va_end(argptr);

	instance->Log_Message(LEVEL::Debug);
	ModuleLock::setSpinLock(instance->m_logLock, false);
}

void Log::Info(const char* format, ...)
{
	if (!instance)
		instance = new Log();
	if (instance->getLogLevel() > LEVEL::Info) {
		return;
	}

	ModuleLock::setSpinLock(instance->m_logLock, true);
	//memset(instance->message, 0, MSG_LENGTH);

	va_list argptr;
	va_start(argptr, format);
	vsnprintf(instance->message, MSG_LENGTH, format, argptr);
	va_end(argptr);

	instance->Log_Message(LEVEL::Info);
	ModuleLock::setSpinLock(instance->m_logLock, false);
}

void Log::Warn(const char* format, ...)
{
	if (!instance)
		instance = new Log();
	if (instance->getLogLevel() > LEVEL::Warn) {
		return;
	}
	ModuleLock::setSpinLock(instance->m_logLock, true);
	//memset(instance->message, 0, MSG_LENGTH);

	va_list argptr;
	va_start(argptr, format);
	vsnprintf(instance->message, MSG_LENGTH, format, argptr);
	va_end(argptr);

	instance->Log_Message(LEVEL::Warn);
	ModuleLock::setSpinLock(instance->m_logLock, false);
}


void Log::Error(const char* format, ...)
{
	if (!instance)
		instance = new Log();

	if (instance->getLogLevel() > LEVEL::Error) {
		return;
	}
	ModuleLock::setSpinLock(instance->m_logLock, true);
	//memset(instance->message, 0, MSG_LENGTH);

	va_list argptr;
	va_start(argptr, format);
	vsnprintf(instance->message, MSG_LENGTH, format, argptr);
	va_end(argptr);

	instance->Log_Message(LEVEL::Error);
	ModuleLock::setSpinLock(instance->m_logLock, false);
}

void Log::Notice(const char * format, ...)
{
	if (!instance)
		instance = new Log();

	if (instance->getLogLevel() > LEVEL::Notice) {
		return;
	}
	ModuleLock::setSpinLock(instance->m_logLock, true);
	//memset(instance->message, 0, MSG_LENGTH);

	va_list argptr;
	va_start(argptr, format);
	vsnprintf(instance->message, MSG_LENGTH, format, argptr);
	va_end(argptr);

	instance->Log_Message(LEVEL::Notice);
	ModuleLock::setSpinLock(instance->m_logLock, false);
}
