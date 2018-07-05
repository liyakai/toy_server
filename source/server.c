#include<stdio.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<time.h>

#include"toyServer_private.h"


int processRequest(void* phInstance);
#define MAXLINE 1024
#define LISTENQ 1024

typedef struct TagSerInstance
{
    int bUseSSL;
    const char* pszServerCert;
	const char* pszServerKey;

	int connfd;
    SSL* ssl;
}tSerInstance;

TOY_SERVER_API int toyServerCreate(TSecSerSetting *tSetting, void** phInstance)
{
	if(!tSetting)
	{
		fprintf(stderr, "toyServerCreate -->> tSetting is NULL");
		return SEC_API_PARAMS_HASNULL;
	}
	*phInstance = (tSerInstance*)malloc(sizeof(tSerInstance));
	memset(*phInstance, 0, sizeof(tSerInstance));
    ((tSerInstance*)(*phInstance)) -> bUseSSL = tSetting -> bUseSSL;
	
	if(tSetting -> pszServerCert)
	{
		((tSerInstance*)(*phInstance)) -> pszServerCert = strdup(tSetting -> pszServerCert);
	}

	if(tSetting -> pszServerKey)
	{
		((tSerInstance*)(*phInstance)) -> pszServerKey = strdup(tSetting -> pszServerKey);
	}
	return 0;
};

TOY_SERVER_API int toyServerDestroy( void* phInstance)
{
	
	if(!phInstance)
	{
		fprintf(stderr, "toyServerDestroy -->> phInstance is NULL");
		return SEC_API_PARAMS_HASNULL;
	}
	if(((tSerInstance*)phInstance) -> pszServerCert)
	{
		free((void*)(((tSerInstance*)phInstance) -> pszServerCert));
		((tSerInstance*)phInstance) -> pszServerCert  = NULL;
	}

	if(((tSerInstance*)phInstance) -> pszServerKey)
	{
		free((void*)(((tSerInstance*)phInstance) -> pszServerKey));
		((tSerInstance*)phInstance) -> pszServerKey  = NULL;
	};
	close(((tSerInstance*)phInstance) -> connfd);
	SSL_free(((tSerInstance*)phInstance) -> ssl);
	free((tSerInstance*)phInstance);
	phInstance = NULL;
    return 0;
};

int toyServer(void*phInstance, int argc, char** argv)
{
	int rv = 0;
   int listenfd, connfd;
//   struct sockaddr_in servaddr, clientaddr;
   struct sockaddr_in  clientaddr;
   socklen_t len;
   pid_t pid;
   char buff[MAXLINE];
   SSL_CTX* ctx;
   if(((tSerInstance*)phInstance) -> bUseSSL)
   {
	   SSL_library_init();
	   ctx = initServerSslCtx();
	   fprintf(stderr,"Server Cert Path:\n%s\n",((tSerInstance*)phInstance) -> pszServerCert);
	   fprintf(stderr,"Server Key Path:\n%s\n",((tSerInstance*)phInstance) -> pszServerKey);
	   rv  = loadCertFile(ctx, ((tSerInstance*)phInstance) -> pszServerCert, ((tSerInstance*)phInstance) -> pszServerKey);
	   if(rv)
	   {
		   return rv;
	   }
   }
   listenfd = openListener(60030);
   for(;;)
   {
       connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &len);
	   ((tSerInstance*)phInstance) -> connfd = connfd;
	   fprintf(stderr, "connection from %s, port %d\n", inet_ntop(AF_INET, &clientaddr.sin_addr, buff,sizeof(buff)), ntohs(clientaddr.sin_port));
	   //fprintf(stderr, "after accept.\n");
	   if(((tSerInstance*)phInstance) -> bUseSSL)
	   {
		   SSL *ssl;
		   ssl = SSL_new(ctx);
		   SSL_set_accept_state(ssl);
		   SSL_set_fd(ssl, connfd);
		   rv = SSL_accept(ssl);
		   if(rv == -1)
		   {
			   fprintf(stderr, "toyServer -->> SSL_accept failed.\n");
			   return SEC_API_SSLACCEPT_FAIL;
		   }
		   ((tSerInstance*)phInstance) -> ssl = ssl;
		   fprintf(stderr, "ser ssl address: %p.\n", (((tSerInstance*)phInstance) -> ssl));
	   }
	   if((pid = fork()) == 0)
	   {
		   close(listenfd);
           processRequest(phInstance);
		   close(connfd);
		   return 0;
	   }
       close(connfd);
   }
   SSL_CTX_free(ctx);
    return 0;
}

int processRequest(void*phInstance)
{
	int rv = 0;
    int i = 0;
    char buff[MAXLINE];
    time_t ticks;
    ticks = time(NULL);
    unsigned long ulErr = 0;
    char szErrMsg[1024] = {0};
    char *pTmp = NULL;
	
	
	fprintf(stderr,"processRequest.\n");
    snprintf(buff, sizeof(buff), "ctime:%s\r\n",ctime(&ticks));
    for(i = 0; i < 1; i++)
	{
	    rv = toySerWrite(phInstance, buff, strlen(buff));
    	if(rv < 0)
	    {
			ulErr = ERR_get_error(); // get err num
            pTmp = ERR_error_string(ulErr,szErrMsg); // 格式：error:errId:库:函数:原因
            fprintf(stderr, ".ulErr:%ld, errno:%d, Reason:%s\n", ulErr, errno, pTmp);
		    fprintf(stderr, "processRequest -->> toySerWrite fail.\n");
		    return SEC_API_SERWRITE_FAIL;
	    }
	}
	if(((tSerInstance*)phInstance) -> ssl)
	{
		fprintf(stderr, "processRequest -->> FREE ssl.\n");
        // SSL_shutdown(((tSerInstance*)phInstance) -> ssl);
        // SSL_free(((tSerInstance*)phInstance) -> ssl);
	}
	return 0;
}

int toySerRead(void*phInstance, void *buf,int num)
{
    if(((tSerInstance*)phInstance) -> bUseSSL)
    {
        return SSL_read(((tSerInstance*)phInstance) -> ssl, buf, num);
    } else
    {
        return read(((tSerInstance*)phInstance) -> connfd, buf, num);
    }
}


int toySerWrite(void*phInstance, void *buf,int num)
{
    if(((tSerInstance*)phInstance) -> bUseSSL)
    {
		fprintf(stderr, "toySerWrite use SSl.\n");
		fprintf(stderr, "ser ssl address: %p.\n", (((tSerInstance*)phInstance) -> ssl));
        return SSL_write(((tSerInstance*)phInstance) -> ssl, buf, num);
    } else
    {
		fprintf(stderr, "toySerWrite NOT use SSl.connfd=%d\n", ((tSerInstance*)phInstance) -> connfd);
        return write(((tSerInstance*)phInstance) -> connfd, buf, num);
    }
}
