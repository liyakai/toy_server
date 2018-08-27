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



TOY_SERVER_API int toyServerCreate(TSecSerSetting *tSetting, void** phInstance)
{
	if(!tSetting)
	{
		LOG_ERROR("toyServerCreate -->> tSetting is NULL");
		return SEC_API_PARAMS_HASNULL;
	}
	*phInstance = (tSerInstance*)malloc(sizeof(tSerInstance));
	memset(*phInstance, 0, sizeof(tSerInstance));
    ((tSerInstance*)(*phInstance)) -> bUseSSL = tSetting -> bUseSSL;
	((tSerInstance*)(*phInstance)) -> bVerifyPeerCert = tSetting -> bVerifyPeerCert;
	
	if(tSetting -> pszServerCert)
	{
		((tSerInstance*)(*phInstance)) -> pszServerCert = strdup(tSetting -> pszServerCert);
	}

	if(tSetting -> pszServerKey)
	{
		((tSerInstance*)(*phInstance)) -> pszServerKey = strdup(tSetting -> pszServerKey);
	}

	if(tSetting -> pszServerCA)
	{
		((tSerInstance*)(*phInstance)) -> pszServerCA = strdup(tSetting -> pszServerCA);
	}
	return 0;
};

TOY_SERVER_API int toyServerDestroy( void* phInstance)
{
	
	if(!phInstance)
	{
		LOG_ERROR("toyServerDestroy -->> phInstance is NULL");
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

	if(((tSerInstance*)phInstance) -> pszServerCA)
	{
		free((void*)(((tSerInstance*)phInstance) -> pszServerCA));
		((tSerInstance*)phInstance) -> pszServerCA  = NULL;
	}
	SSL_free(((tSerInstance*)phInstance) -> ssl);
	close(((tSerInstance*)phInstance) -> connfd);
	free((tSerInstance*)phInstance);
	phInstance = NULL;
    return 0;
};

int toyServer(void*phInstance, int argc, char** argv)
{
   int rv = 0, i = 0;
   int listenfd, connfd, maxi, sockfd; //, maxfd;
   int nready;
   // fd_set rset, allset;
   //struct sockaddr_in servaddr, clientaddr;
   struct sockaddr_in  clientaddr;
   socklen_t clientLen;
   //pid_t pid;
   char buff[MAXLINE];
   SSL_CTX* ctx;
   if(((tSerInstance*)phInstance) -> bUseSSL)
   {
	   SSL_library_init();
	   ctx = initServerSslCtx();
	   LOG_INFO("Server Cert Path:\n%s\n",((tSerInstance*)phInstance) -> pszServerCert);
	   LOG_INFO("Server Key Path:\n%s\n",((tSerInstance*)phInstance) -> pszServerKey);
	   LOG_INFO("Server CA Path:\n%s\n",((tSerInstance*)phInstance) -> pszServerCA);
	   rv  = loadCertFile(ctx, ((tSerInstance*)phInstance) -> pszServerCert, ((tSerInstance*)phInstance) -> pszServerKey, ((tSerInstance*)phInstance) -> pszServerCA);
	   if(rv)
	   {
		   return rv;
	   }
   }
   	if(((tSerInstance*)phInstance) -> bVerifyPeerCert)
	{
		SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL);
    }
   if(argc < 2)
   {
       LOG_ERROR("Error:short of port.\n");
       return -1;
   }
   listenfd = openListener(atoi(argv[1]));

   // maxfd = listenfd; // use for select
   maxi = 0;
   ((tSerInstance*)phInstance) -> clientPoll[0].fd = listenfd;
   ((tSerInstance*)phInstance) -> clientPoll[0].events = POLLRDNORM;
   for(i = 0; i< FD_SETSIZE; i++)
   {
	   ((tSerInstance*)phInstance) -> clientSocket[i] = -1;
	   ((tSerInstance*)phInstance) -> clientSSL[i] = NULL;
	   ((tSerInstance*)phInstance) -> clientPoll[i].fd = -1;
   }
   // FD_ZERO(&allset);
   // FD_SET(listenfd, &allset);

   for(;;)
   {
#if 0 // select
	   rset = allset;
	   LOG_INFO("before select: select one.\n");
	   nready = select(maxfd+1, &rset, NULL, NULL, NULL);
	   if(FD_ISSET(listenfd, &rset))
	   {
		    clientLen = sizeof(clientaddr);
		    // LOG_INFO("after select: select one. nready is %d\n", nready);
			connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &clientLen);
			// LOG_INFO("after select: connfd is %d.\n", connfd);
			for(i = 0; i < FD_SETSIZE; i++)
			{
				if(((tSerInstance*)phInstance) -> clientSocket[i] < 0)
				{
					((tSerInstance*)phInstance) -> clientSocket[i] = connfd;
					LOG_DEBUG("((tSerInstance*)phInstance) -> clientSocket[%d] is %d.\n", i, ((tSerInstance*)phInstance) -> clientSocket[i]);
					break;
				}
			}
			if(i == FD_SETSIZE)
			{
				LOG_ERROR("Error: too many clients.\n");
				return -1;
			}
			FD_SET(connfd, &allset);
			if(connfd > maxfd)
			{
				maxfd = connfd;
			}
			LOG_DEBUG("maxi=%d, i = %d.\n", maxi, i);
			if(i > maxi)
			{
				maxi = i;
			}
			//((tSerInstance*)phInstance) -> connfd = connfd;
			LOG_INFO("connection from %s, port %d\n", inet_ntop(AF_INET, &clientaddr.sin_addr, buff,sizeof(buff)), ntohs(clientaddr.sin_port));
			//fprintf(stderr, "after accept.\n");
#endif
        nready = poll(((tSerInstance*)phInstance) -> clientPoll, maxi + 1, INFTIM);
		if(((tSerInstance*)phInstance) -> clientPoll[0].revents & POLLRDNORM)
		{
			clientLen = sizeof(clientaddr);
		    // LOG_INFO("after select: select one. nready is %d\n", nready);
			connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &clientLen);
			// LOG_INFO("after select: connfd is %d.\n", connfd);
			for(i = 0; i < OPEN_MAX; i++)
			{
				if(((tSerInstance*)phInstance) -> clientPoll[i].fd < 0)
				{
					((tSerInstance*)phInstance) -> clientPoll[i].fd = connfd;
					LOG_DEBUG("((tSerInstance*)phInstance) -> clientPoll[%d] is %d.\n", i, ((tSerInstance*)phInstance) -> clientSocket[i]);
					break;
				}
			}
			if(i == OPEN_MAX)
			{
				LOG_ERROR("Error: too many clients.\n");
				return -1;
			}
			((tSerInstance*)phInstance) -> clientPoll[i].events = POLLRDNORM;
			if(i > maxi)
			{
				maxi = i;
			}
		
			if(((tSerInstance*)phInstance) -> bUseSSL)
			{
				SSL *ssl;
				ssl= SSL_new(ctx);
				SSL_set_accept_state(ssl);
				SSL_set_fd(ssl, connfd );
				rv = SSL_accept(ssl);
				if(rv != 1)
				{
					if(errno == EINTR)
					{
						LOG_DEBUG(" SSL_accept errno == EINTR \n");
						continue; // back to for()
					} else
					{
							LOG_ERROR("toyServer -->> SSL_accept failed.\n");
							return SEC_API_SSLACCEPT_FAIL;
					}
				}
				LOG_INFO("ssl handshake.\n");
				showCerts(ssl);
				sprintf(buff, "ni hao ShangHai.");
				//SSL_write(ssl, buff, strlen(buff));
				//SSL_write(ssl, buff, 0);
				//((tSerInstance*)phInstance) -> ssl = ((tSerInstance*)phInstance) -> clientSSL[i];
				((tSerInstance*)phInstance) -> clientSSL[i] = ssl;
				//LOG_INFO("ser ssl address: %p.\n", (((tSerInstance*)phInstance) -> ssl));
			}
			if(--nready <= 0)
			{
				continue;
			}
	   }
	   
#if 0
	   if((pid = fork()) == 0)
	   {
		   close(listenfd);
           processRequest(phInstance);
		   close(connfd);
		   return 0;
	   }
	   LOG_INFO("child thread pid:%d\n",pid);
       close(connfd);
   }
#endif
#if 0 // select
		for(i = 0; i <= maxi; i++)
		{
			if((sockfd = ((tSerInstance*)phInstance) -> clientSocket[i]) < 0)
			{
				LOG_DEBUG("sockfd < 0\n");
				continue;
			}
			if(FD_ISSET(sockfd, &rset))
			{
				((tSerInstance*)phInstance) -> connfd = ((tSerInstance*)phInstance) -> clientSocket[i];
				((tSerInstance*)phInstance) -> ssl = ((tSerInstance*)phInstance) -> clientSSL[i];
				rv = processRequest(phInstance);
				if(rv)
				{
					SSL_free(((tSerInstance*)phInstance) -> clientSSL[i]);
	                close(((tSerInstance*)phInstance) -> clientSocket[i]);
					FD_CLR(sockfd, &allset);
					((tSerInstance*)phInstance) -> clientSocket[i] = -1;
				}
				if(--nready <= 0)
				{
					break;
				}
			}

		}
#endif
		for(i = 0; i <= maxi; i++)
		{
			if((sockfd = ((tSerInstance*)phInstance) -> clientPoll[i].fd) < 0)
			{
				LOG_DEBUG("sockfd < 0\n");
				continue;
			}
			if(((tSerInstance*)phInstance) -> clientPoll[i].revents & (POLLRDNORM | POLLERR))
			{
				((tSerInstance*)phInstance) -> connfd = ((tSerInstance*)phInstance) -> clientPoll[i].fd;
				((tSerInstance*)phInstance) -> ssl = ((tSerInstance*)phInstance) -> clientSSL[i];
				rv = processRequest(phInstance);
				if(rv)
				{
					SSL_free(((tSerInstance*)phInstance) -> clientSSL[i]);
	                close(((tSerInstance*)phInstance) ->  clientPoll[i].fd);
					((tSerInstance*)phInstance) -> clientPoll[i].fd = -1;
				}
				if(--nready <= 0)
				{
					break;
				}
			}

		}
    }
    SSL_CTX_free(ctx);
    return 0;
}
int processRequest(void*phInstance)
{
	int rv = 0;
    // int i = 0;
    char buff[MAXLINE];
	int n = MAXLINE;
    // time_t ticks;
    // ticks = time(NULL);
    // unsigned long ulErr = 0;
    // char szErrMsg[1024] = {0};
    // char *pTmp = NULL;
	
	
	LOG_INFO("processRequest.\n");
#if 0
    snprintf(buff, sizeof(buff), "ctime:%s\r\n",ctime(&ticks));
    for(i = 0; i < 1; i++)
	{
	    rv = toySerWrite(phInstance, buff, strlen(buff));
    	if(rv < 0)
	    {
			// ulErr = ERR_get_error(); // get err num
            // pTmp = ERR_error_string(ulErr,szErrMsg); // 格式：error:errId:库:函数:原因
            // LOG_ERROR(".ulErr:%ld, errno:%d, Reason:%s\n", ulErr, errno, pTmp);
		    // LOG_ERROR("processRequest -->> toySerWrite fail.\n");
		    return SEC_API_SERWRITE_FAIL;
	    } else 
		{
			LOG_INFO("processRequest -->> toySerWrite success.\n");
		}
	}
	if(((tSerInstance*)phInstance) -> ssl)
	{
		// LOG_INFO("processRequest -->> FREE ssl.\n");
        // SSL_shutdown(((tSerInstance*)phInstance) -> ssl);
        // SSL_free(((tSerInstance*)phInstance) -> ssl);
	}
#endif // if 0
    //while(n > 0)
	{
		memset(buff, 0, sizeof(buff));
		if((n = toySerRead(phInstance, buff, sizeof(buff))) > 0)
		{
			LOG_INFO("processRequest -->> read from client(len:%d):\n%s\n", n, buff);
			toySerWrite(phInstance, buff, n);
			memset(buff, 0, sizeof(buff));
		}
		if(n < 0 && errno == EINTR)
		{
			n = -n;
			LOG_ERROR("processRequest -->> n < 0 && errno == EINTR.\n");
		} else if (n < 0)
		{
			LOG_ERROR("processRequest -->> toySerRead fail.\n");
			rv = SEC_CONN_SERVER_READ_FAIL;
		}
		if(n == 0)
		{
			rv = SEC_CONN_CLOSED_BY_CLIENT;
		}
	}

	return rv;
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
		// LOG_DEBUG("toySerWrite use SSl.\n");
		// LOG_DEBUG("ser ssl address: %p.\n", (((tSerInstance*)phInstance) -> ssl));
        return SSL_write(((tSerInstance*)phInstance) -> ssl, buf, num);
    } else
    {
		// fprintf(stderr, "toySerWrite NOT use SSl.connfd=%d\n", ((tSerInstance*)phInstance) -> connfd);
        return write(((tSerInstance*)phInstance) -> connfd, buf, num);
    }
}
