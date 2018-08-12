#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>

#include "toyServer_private.h"

#define MAXLINE (1024)


TOY_SERVER_API int toyClientSessionCreate(TSecCliSetting *tSetting, void** phSession)
{
	if(tSetting == NULL)
	{
		LOG_ERROR("toyClientSessionCreate -->> tSetting is NULL");
        return SEC_API_PARAMS_HASNULL;
	}
	*phSession =  (tCliSession*)malloc(sizeof(tCliSession));
	memset(*phSession, 0, sizeof(tCliSession));
	((tCliSession*)(*phSession)) -> bUseSSL = tSetting -> bUseSSL;
	((tCliSession*)(*phSession)) -> bVerifyPeerCert = tSetting -> bVerifyPeerCert;
	if(tSetting -> pszClientCert)
	{
		((tCliSession*)(*phSession)) -> pszClientCert = strdup(tSetting -> pszClientCert);
	}

	if(tSetting -> pszClientKey)
	{
		((tCliSession*)(*phSession)) -> pszClientKey = strdup(tSetting -> pszClientKey);
	}

	if(tSetting -> pszClientCA)
	{
		((tCliSession*)(*phSession)) -> pszClientCA = strdup(tSetting -> pszClientCA);
	}

	return 0;
};
TOY_SERVER_API int toyClientSessionDestroy( void* phSession)
{

	if(phSession == NULL)
	{
		LOG_ERROR("toyClientSessionCreate -->> phSession is NULL");
        return SEC_API_PARAMS_HASNULL;
	}
	if(((tCliSession*)phSession) -> ssl)
	{
		LOG_ERROR("toyClientSessionDestroy-->> ssl address:%p\n",((tCliSession*)phSession) -> ssl);
		SSL_shutdown(((tCliSession*)phSession) -> ssl);
		SSL_free(((tCliSession*)phSession) -> ssl);
		((tCliSession*)phSession) -> ssl = NULL;
	}

	if(((tCliSession*)phSession) -> pszClientCert)
	{
		free((void*)(((tCliSession*)phSession) -> pszClientCert));
		((tCliSession*)phSession) -> pszClientCert  = NULL;
	}
	if(((tCliSession*)phSession) -> pszClientKey)
	{
		free((void*)(((tCliSession*)phSession) -> pszClientKey));
		((tCliSession*)phSession) -> pszClientKey  = NULL;
	}
	if(((tCliSession*)phSession) -> pszClientCA)
	{
		free((void*)(((tCliSession*)phSession) -> pszClientCA));
		((tCliSession*)phSession) -> pszClientCA  = NULL;
	}

	free( (tCliSession*)phSession);
	phSession = NULL;
	return 0;
};


int toyClient(void*phSession, int argc, char** argv)
{
    int sockfd;
	int  n;
    char recvline[MAXLINE+1];
    unsigned long ulErr = 0;
    char szErrMsg[1024] = {0};
    char *pTmp = NULL;
	 
    if(argc < 2)
    {
        LOG_ERROR("usage:xxx IPaddr port\n");
    }

	sockfd =  openConnection(argv[1], atoi(argv[2]));
    ((tCliSession*)phSession) -> sockfd = sockfd;
	if(((tCliSession*)phSession) -> bUseSSL)
	{
		((tCliSession*)phSession) -> ssl = getCliSsl(phSession, sockfd);
		if(!((tCliSession*)phSession) -> ssl)
		{
			LOG_ERROR("generate SSL  from sockfd failed.\n ");
			return -1;
		}
	}
    // fprintf(stderr, "###########toyClient  tag 5\n");
	while((n = toyCliRead(phSession, recvline, MAXLINE)) > 0)
    {
        recvline[n] = 0;
        //fprintf(stderr, "toyCliRead;  n = %d\n",n);
		//fprintf(stderr, "received:%s\n",recvline);
        if(fputs(recvline, stdout) == EOF)
        {
	        LOG_ERROR("fputs error");
			return -1;
	    }
    }
    if( n < 0)
    {
		ulErr = ERR_get_error(); // get err num
        pTmp = ERR_error_string(ulErr,szErrMsg); // 格式：error:errId:库:函数:原因
        ERR_print_errors_fp(stderr);
		LOG_ERROR("toyClient -->> toyCliRead  error.n = %d, ulErr:%ld, errno:%d, errnoStr:%s,  Reason:%s\n", n, ulErr, errno, strerror(errno), pTmp);
		return -1;
    }
	if(((tCliSession*)phSession) -> bUseSSL)
	{
	    SSL_shutdown(((tCliSession*)phSession) -> ssl);
        SSL_free(((tCliSession*)phSession) -> ssl);
		((tCliSession*)phSession) -> ssl = NULL;
	} else 
	{
		close(((tCliSession*)phSession) -> sockfd);
		((tCliSession*)phSession) -> sockfd = 0;
	}
    return (0);
}

int toyCliRead(void*phSession, void *buf,int num)
{
	if(((tCliSession*)phSession) -> bUseSSL)
	{
//		fprintf(stderr, "toyCliRead use SSL.\n");
//		fprintf(stderr, "cli ssl address: %p.\n", (((tCliSession*)phSession) -> ssl));
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
