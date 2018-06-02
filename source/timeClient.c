#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>

#define MAXLINE (1024)

int main(int argc, char** argv)
{
    int sockfd, n;
    char recvline[MAXLINE+1];
    struct sockaddr_in servaddr;

    if(argc != 2)
    {
      fprintf(stderr, "usage: a.out <IPaddress>");  
	  return -1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "socket error.");
		return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(60030);    // daytime server
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
    {
        fprintf(stderr, "inet_pton error for %s", argv[1]);
		return -1;
    }

    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        fprintf(stderr, "connect error.");
		return -1;
    }
    while((n = read(sockfd, recvline, MAXLINE)) > 0)
    {
        recvline[n] = 0;
        if(fputs(recvline, stdout) == EOF)
        {
	        fprintf(stderr, "fputs error");
			return -1;
	    }
    }
    if( n < 0)
    {
        fprintf(stderr, "read error");
		return -1;
    }
    return (0);
}
