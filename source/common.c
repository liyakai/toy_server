#include "toyServer_private.h"
#include <string.h>
FuncLibLogWrite toyLog = NULL;
void* toyLogHandle = NULL;
TOY_SERVER_API int toySetLogCB(void *hLog, FuncLibLogWrite pfLogWrite)
{
	toyLogHandle = hLog;
	toyLog = pfLogWrite;
	LOG_INFO("Log callback has been setted.");
	return 0;
};

int toyLogVar(void* logHandle, int level, const char *fmt, ...)
{
	int ret = 0;
    char logStr[4096];
    va_list ap;
    va_start(ap, fmt);
    memset(logStr, 0, sizeof(logStr));
    vsnprintf(logStr, sizeof(logStr), fmt, ap);
    va_end(ap);
	toyLog(logHandle, level, logStr);
    return ret;
};