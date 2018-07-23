#ifndef TOY_SERVER_PRIVATE
#define TOY_SERVER_PRIVATE

#include "toyServer.h"
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

#define LOG(logLevel, message,...) \
        if(toyLog) \
        { \
            toyLogVar(toyLogHandle, logLevel, message, ##__VA_ARGS__); \
        }

#define LOG_FATAL(message,...) LOG(COM_LOG_LEVEL_FATAL, message, ##__VA_ARGS__)
#define LOG_ERROR(message,...) LOG(COM_LOG_LEVEL_ERROR, message, ##__VA_ARGS__)
#define LOG_WARN(message,...) LOG(COM_LOG_LEVEL_WARN, message, ##__VA_ARGS__)
#define LOG_INFO(message,...) LOG(COM_LOG_LEVEL_INFO, message, ##__VA_ARGS__)
#define LOG_DEBUG(message,...) LOG(COM_LOG_LEVEL_DEBUG, message, ##__VA_ARGS__)
#define LOG_TRACE(message,...) LOG(COM_LOG_LEVEL_TRACE, message, ##__VA_ARGS__)

typedef struct TagSerSession
{
    int bUseSSL;
	const char* pszClientCert;
	const char* pszClientKey;
	const char* pszClientCA;
	int sockfd;
	SSL* ssl;
}tCliSession;

typedef struct TagSerInstance
{
    int bUseSSL;
    const char* pszServerCert;
	const char* pszServerKey;
	const char* pszServerCA;

	int connfd;
    SSL* ssl;
}tSerInstance;

// log function
int toyLogVar(void* logHandle, int level, const char *fmt, ...);

int openListener(int port);
SSL_CTX* initServerSslCtx(void);
int loadCertFile(SSL_CTX* ctx, const char* CertFile, const char* KeyFile, const char* caFile);
int showCerts(SSL* ssl);

int openConnection(const char* ipAddr, int port);
SSL_CTX* initCliSslCtx(void);
SSL* getCliSsl(void*phSession, int sockfd);

int toyCliRead(void*phSession, void *buf,int num);
int toyCliWrite(void*phSession, void *buf,int num);

int toySerRead(void*phInstance, void *buf,int num);
int toySerWrite(void*phInstance, void *buf,int num);

#endif  // TOY_SERVER_PRIVATE
