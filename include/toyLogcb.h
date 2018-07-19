
#ifndef TOYLOGCB_H
#define TOYLOGCB_H

// LOG LEVEL, THE SAME WITH LOG4CPLUS
#define COM_LOG_LEVEL_OFF 	60000
#define COM_LOG_LEVEL_FATAL 50000
#define COM_LOG_LEVEL_ERROR 40000
#define COM_LOG_LEVEL_WARN 	30000
#define COM_LOG_LEVEL_INFO 	20000
#define COM_LOG_LEVEL_DEBUG 10000
#define COM_LOG_LEVEL_TRACE 0
#define COM_LOG_LEVEL_ALL 	COM_LOG_LEVEL_TRACE
/************* the mappling relations between log4cplus and kdvlog *******************
	here					log4cplus			
	COM_LOG_LEVEL_OFF		OFF_LOG_LEVEL		
	COM_LOG_LEVEL_FATAL		FATAL_LOG_LEVEL		
	COM_LOG_LEVEL_ERROR		ERROR_LOG_LEVEL	
	COM_LOG_LEVEL_WARN		WARN_LOG_LEVEL		
	COM_LOG_LEVEL_INFO		INFO_LOG_LEVEL		
	COM_LOG_LEVEL_DEBUG		DEBUG_LOG_LEVEL		
	COM_LOG_LEVEL_TRACE		TRACE_LOG_LEVEL	
	COM_LOG_LEVEL_ALL		ALL_LOG_LEVEL	
************* LogLevel should be completed by the caller ******************/

/*=============================================================================
function name	£∫	FuncLibLogWrite
function	£∫	set log callback function
parameters£∫	hLog			[IN]the log handle£¨set bt the caller
               nLevel           [IN]log level
			   szMessage		[IN]log message
return value£∫0		success
		 ~0		  error code
=============================================================================*/
typedef int (*FuncLibLogWrite)(void *hLog, int nLevel, const char *szMessage);

// int setLogCB(void *hLog, FuncLibLogWrite pfLogWrite);

#ifdef COM_LOG_UNIT_TEST	//untested
#if 0 //log4plus demo
#include <string>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/hierarchy.h>

using std::string;
using namespace log4cplus;
using log4cplus::PropertyConfigurator;
using log4cplus::Logger;

static s32 MyLogWrite( void *hLog, s32 nLevel, const char *szMessage)
{
	if(!hLog) return 0;
	Logger *myLog = (Logger *)hLog;

	myLog->log(nLevel, szMessage);

	return 0;
}

static Logger g_log;

void main()
{
	s32 nRetVal = 0;

	string conf_file = "my.conf";
	const char *cat_name = "KdvSecBiz";

	log4cplus::initialize(); 
	PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(conf_file));
	g_log = Logger::getInstance(cat_name);

	nRetVal = KdvSecBizInitEx(&g_log, MyLogWrite);

	//......
}
#else 

static s32 MyLogWrite(void *hLog, s32 nLevel, const char *szMessage)
{
	if(!hLog) return 0;
	u16 wModule = (u16)hLog;

	switch(nLevel)	//LogLevel”≥…‰
	{
	case COM_LOG_LEVEL_ALL:
	case COM_LOG_LEVEL_TRACE:
		LogPrint( LOG_LVL_REPEAT, wModule, "%s", szMessage);
		break;

	case COM_LOG_LEVEL_DEBUG:
		LogPrint( LOG_LVL_DETAIL, wModule, "%s", szMessage);
		break;

	case COM_LOG_LEVEL_INFO:
		LogPrint( LOG_LVL_KEYSTATUS, wModule, "%s", szMessage);
		break;

	case COM_LOG_LEVEL_WARN:
		LogPrint( LOG_LVL_WARNING, wModule, "%s", szMessage);
		break;

	case COM_LOG_LEVEL_ERROR:
	case COM_LOG_LEVEL_FATAL:
		LogPrint( LOG_LVL_ERROR, wModule, "%s", szMessage);
		break;

	case COM_LOG_LEVEL_OFF:
	case default:
		break;
	}
	return 0;
}

const u16 wSecBizModule = 1;

void main()
{
	s32 nRetVal = 0;

	Err_t err;
	const s8 *pszCfgFileFullPath = "my.conf";

	err = LogInit(pszCfgFileFullPath);

	nRetVal = KdvSecBizInitEx((void *)wSecBizModule, MyLogWrite);

	//......

	LogQuit();
}
#endif

#endif //COM_LOG_UNIT_TEST

#endif	//TOYLOGCB_H
