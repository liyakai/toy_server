#include<stdio.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<time.h>

#include"toyServer.h"


int processRequest(int connfd);
#define MAXLINE 1024
#define LISTENQ 1024
int toyServer(int argc, char** argv)
{
   int listenfd, connfd;
   struct sockaddr_in servaddr, clientaddr;
   socklen_t len;
   pid_t pid;
   char buff[MAXLINE];

   listenfd = socket(AF_INET, SOCK_STREAM, 0);

   memset(&servaddr, 0, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servaddr.sin_port=htons(60030);

   bind(listenfd, (const struct sockaddr*)&servaddr, sizeof(servaddr));
   listen(listenfd, LISTENQ);

   for(;;)
   {
       connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &len);
	   fprintf(stderr, "connection from %s, port %d\n", inet_ntop(AF_INET, &clientaddr.sin_addr, buff,sizeof(buff)), ntohs(clientaddr.sin_port));
	   if((pid = fork()) == 0)
	   {
		   close(listenfd);
           processRequest(connfd);
		   close(connfd);
		   return 0;
	   }
       close(connfd);
   }
    return 0;
}

int processRequest(int connfd)
{
    char buff[MAXLINE];
    time_t ticks;
    ticks = time(NULL);
    snprintf(buff, sizeof(buff), "ctime:%s\r\n",ctime(&ticks));
    write(connfd, buff, strlen(buff));
	return 0;
}

