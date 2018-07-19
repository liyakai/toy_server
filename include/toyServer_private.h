#ifndef TOY_SERVER_PRIVATE
#define TOY_SERVER_PRIVATE

#include "toyServer.h"
#include "toyLogcb.h"
#include "toyServerErrcode.h"
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
typedef struct TagSrvCtx{
}tSrvCtx;

extern FuncLibLogWrite toyLog;
extern void* toyLogHandle;

#define LOG(logLevel, message) \
        if(toyLog) \
        { \
            toyLog(toyLogHandle, logLevel, message); \
        }

#define LOG_FATAL(message) LOG(COM_LOG_LEVEL_FATAL, message)
#define LOG_ERROR(message) LOG(COM_LOG_LEVEL_ERROR, message)
#define LOG_WARN(message) LOG(COM_LOG_LEVEL_WARN, message)
#define LOG_INFO(message) LOG(COM_LOG_LEVEL_INFO, message)
#define LOG_DEBUG(message) LOG(COM_LOG_LEVEL_DEBUG, message)
#define LOG_TRACE(message) LOG(COM_LOG_LEVEL_TRACE, message)

int openListener(int port);
SSL_CTX* initServerSslCtx(void);
int loadCertFile(SSL_CTX* ctx, const char* CertFile, const char* KeyFile);
int showCerts(SSL* ssl);

int openConnection(const char* ipAddr, int port);
SSL_CTX* initCliSslCtx(void);
SSL* getCliSsl(int sockfd);

int toyCliRead(void*phSession, void *buf,int num);
int toyCliWrite(void*phSession, void *buf,int num);

int toySerRead(void*phInstance, void *buf,int num);
int toySerWrite(void*phInstance, void *buf,int num);

#endif  // TOY_SERVER_PRIVATE
