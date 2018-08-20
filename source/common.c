#include "toyServer_private.h"
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
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

Sigfunc* signal(int signo, Sigfunc *func)
{
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT; // SunOS 4.x
#endif
    } else 
    {
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART;    // SVR$, 4.4BSD
#endif
    }
    if(sigaction(signo, &act, &oact) < 0)
    {
        return (SIG_ERR);
    }
    return (oact.sa_handler);
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;
    while((pid = waitpid(-1, &stat, WNOHANG))>0)
    {
        LOG_INFO("child %d terminated.\n", pid);
    }
    return;
}