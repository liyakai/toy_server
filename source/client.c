#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>

#include "toyServer_private.h"

#define MAXLINE (1024)
typedef struct TagSerSession
{
    int bUseSSL;
	int sockfd;
	SSL* ssl;
}tCliSession;

TOY_SERVER_API int toyClientSessionCreate(TSecCliSetting *tSetting, void** phSession)
{
	if(tSetting == NULL)
	{
		fprintf(stderr, "toyClientSessionCreate -->> tSetting is NULL");
        return SEC_API_PARAMS_HASNULL;
	}
	*phSession =  (tCliSession*)malloc(sizeof(tCliSession));
	memset(*phSession, 0, sizeof(tCliSession));
	((tCliSession*)(*phSession)) -> bUseSSL = tSetting -> bUseSSL;
	return 0;
};
TOY_SERVER_API int toyClientSessionDestroy( void* phSession)
{

	if(phSession == NULL)
	{
		fprintf(stderr, "toyClientSessionCreate -->> phSession is NULL");
        return SEC_API_PARAMS_HASNULL;
	}
	if(((tCliSession*)phSession) -> ssl)
	{
		SSL_free(((tCliSession*)phSession) -> ssl);
	}
	free( (tCliSession*)phSession);
	phSession = NULL;
	return 0;
};


int toyClient(void*phSession, int argc, char** argv)
{
    int sockfd, n;
    char recvline[MAXLINE+1];
    unsigned long ulErr = 0;
    char szErrMsg[1024] = {0};
    char *pTmp = NULL;
	 
    if(argc < 2)
    {
        fprintf(stderr, "usage:xxx IPaddr port\n");
    }

	sockfd =  openConnection(argv[1], atoi(argv[2]));
    ((tCliSession*)phSession) -> sockfd = sockfd;
	if(((tCliSession*)phSession) -> bUseSSL)
	//if(1)
	{
	    SSL_CTX *ssl_ctx;
	    SSL *ssl;
	    ssl_ctx = initCliSslCtx();
		if(!ssl_ctx)
		{
			fprintf(stderr, "toyClient -->> initCliSslCtx failed.\n");
		}
	    ssl = SSL_new(ssl_ctx);    // create new SSL connection state
		if(!ssl)
		{
			fprintf(stderr, "toyClient -->> SSL_new failed.\n");
		}
		SSL_set_connect_state(ssl);
	    SSL_set_fd(ssl, sockfd);   // attache socet descriptor
	    if(SSL_connect(ssl)  == -1)   // perform the connection
	    {
		    fprintf(stderr, "toyClient -->> SSL_connect failed\n");
		    return -1;
	    } else 
		{
			printf("toyClient -->> Connected with %s encryption\n", SSL_get_cipher(ssl));
		    showCerts(ssl);
		}
		((tCliSession*)phSession) -> ssl = ssl;
        fprintf(stderr, "cli ssl address: %p.\n", (((tCliSession*)phSession) -> ssl));
		SSL_CTX_free(ssl_ctx);
	}
    fprintf(stderr, "###########toyClient  tag 5\n");
	while((n = toyCliRead(phSession, recvline, MAXLINE)) > 0)
    //while((n = SSL_read(((tCliSession*)phSession) -> ssl, recvline, MAXLINE)) > 0)
    {
        recvline[n] = 0;
        fprintf(stderr, "###########toyClient  tag 6\n");
        if(fputs(recvline, stdout) == EOF)
        {
	        fprintf(stderr, "fputs error");
			return -1;
	    }
    }
    if( n < 0)
    {
		ulErr = ERR_get_error(); // get err num
        pTmp = ERR_error_string(ulErr,szErrMsg); // 格式：error:errId:库:函数:原因
        ERR_print_errors_fp(stderr);
		fprintf(stderr, "toyClient -->> toyCliRead  error.n = %d, ulErr:%ld, errno:%d, errnoStr:%s,  Reason:%s\n", n, ulErr, errno, strerror(errno), pTmp);
		return -1;
    }
	if(((tCliSession*)phSession) -> bUseSSL)
	{
	    SSL_shutdown(((tCliSession*)phSession) -> ssl);
        SSL_free(((tCliSession*)phSession) -> ssl);
	} else 
	{
		close(((tCliSession*)phSession) -> sockfd);
	}
    return (0);
}

int toyCliRead(void*phSession, void *buf,int num)
{
	if(((tCliSession*)phSession) -> bUseSSL)
	{
		fprintf(stderr, "toyCliRead use SSL.\n");
		fprintf(stderr, "cli ssl address: %p.\n", (((tCliSession*)phSession) -> ssl));
		return SSL_read(((tCliSession*)phSession) -> ssl, buf, num);
	} else 
	{
		fprintf(stderr, "toyCliRead.sockfd=%d\n", ((tCliSession*)phSession) -> sockfd);
		return read(((tCliSession*)phSession) -> sockfd, buf, num);
	}
}


int toyCliWrite(void*phSession, void *buf,int num)
{
	if(((tCliSession*)phSession) -> bUseSSL)
	{ 
		return SSL_write(((tCliSession*)phSession) -> ssl, buf, num);
	} else 
	{
		return write(((tCliSession*)phSession) -> sockfd, buf, num);
	}
}
