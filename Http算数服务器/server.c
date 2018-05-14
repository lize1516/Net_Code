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

int startup(int _port)
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
    server_socket.sin_addr.s_addr=htonl(INADDR_ANY);
    server_socket.sin_port=htons(_port);

    if(bind(sock,(struct sockaddr*)&server_socket,sizeof(struct sockaddr_in))<0)
    {
        perror("bind");
        close(sock);
        return 3;
    }

    if(listen(sock,_BACKLOG)<0)
    {
        perror("listen");
        close(sock);
        return 4;
    }
    printf("bind and listen success,wait...\n");

    return sock;
}

void serviceFun(int sock)
{
    for(;;)
    {
        Request request;
        read(sock,&request.a,sizeof(request));
        Response response;
        response.sum=request.a+request.b;
        write(sock,&response,sizeof(response));
    }
    close(sock);
}

int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        printf("usage");
        return 1;
    }
    int _port=atoi(argv[2]);
    int sock=startup(_port);
    
    struct sockaddr_in client_socket;
    for( ; ; )
    {
        socklen_t len=0;
        int client_sock=accept(sock,(struct sockaddr*)&client_socket,&len);
        if(client_sock<0)
        {
            perror("accept");
            close(sock);
            return 5;
        }      
        char buf_ip[INET_ADDRSTRLEN];
        memset(buf_ip,0,sizeof(buf_ip));
        inet_ntop(AF_INET,&client_socket.sin_addr,buf_ip,sizeof(buf_ip));
        printf("get connect,ip is: %s ,port is: %d\n",buf_ip,ntohs(client_socket.sin_port));

        serviceFun(client_sock);
    }
    close(sock);
    return 0;
}
