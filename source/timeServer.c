#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<time.h>

#define MAXLINE 1024
#define LISTENQ 1024
int main(int argc, char** argv)
{
   int listenfd, connfd;
   struct sockaddr_in servaddr;
   char buff[MAXLINE];
   time_t ticks;

   listenfd = socket(AF_INET, SOCK_STREAM, 0);

   memset(&servaddr, 0, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servaddr.sin_port=htons(60030);

   bind(listenfd, (struct socksddr*)&servaddr, sizeof(servaddr));
   listen(listenfd, LISTENQ);

   for(;;)
   {
       connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
       ticks = time(NULL);
       snprintf(buff, sizeof(buff), "ctime:%s\r\n",ctime(&ticks));
       write(connfd, buff, strlen(buff));

       close(connfd);
   }
    return 0;
}
