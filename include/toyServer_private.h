#ifndef TOY_SERVER_PRIVATE
#define TOY_SERVER_PRIVATE

#include "toyServer.h"
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
typedef struct TagSrvCtx{

}tSrvCtx;

int openListener(int port);
SSL_CTX* InitServerSslCtx(void);
int loadCertFile(SSL_CTX* ctx, const char* CertFile, const char* KeyFile);
int showCerts(SSL* ssl);

int openConnection(const char* ipAddr, int port);


#endif  // TOY_SERVER_PRIVATE
