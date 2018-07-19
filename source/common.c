#include "toyServer_private.h"
FuncLibLogWrite toyLog = NULL;
void* toyLogHandle = NULL;
TOY_SERVER_API int toySetLogCB(void *hLog, FuncLibLogWrite pfLogWrite)
{
	toyLogHandle = hLog;
	toyLog = pfLogWrite;
	return 0;
};