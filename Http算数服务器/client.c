#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include"proto.h"

#define _BACKLOG 10

int startup(int _port,char* addr)
{
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        perror("sock");
        return 2;
    }
    struct sockaddr_in server_socket;
    bzero(&server_socket,sizeof(server_socket));
    server_socket.sin_family=AF_INET;
    inet_pton(AF_INET,addr,&server_socket.sin_addr);
    server_socket.sin_port=htons(_port);
    
    int ret=connect(sock,(struct sockaddr*)&server_socket,sizeof(struct sockaddr_in));
    if(ret<0)
    {
        perror("connect");
        close(sock);
        return 3;
    }
    printf("connect success ...\n");

    return sock;
}

void service(int sock)
{
    for(;;)
    {
        Request request;
        Response response;
        printf("please input [a] and [b]:");
        scanf("%lf%lf",&request.a,&request.b);
        write(sock,&request,sizeof(request));
        read(sock,&response,sizeof(response));
        printf("%lf+%lf=%lf\n",request.a,request.b,response.sum);
    }
}

int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        printf("usage");
        return 1;
    }
    int _port=atoi(argv[2]);
    int sock=startup(_port,argv[1]);
    
    service(sock);

    close(sock);
    return 0;
}
