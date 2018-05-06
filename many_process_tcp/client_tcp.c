#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>

#define _PORT 8080


void serviceFun(int client_sock)
{
    while(1)
    {
        printf("client#:");
        char buf[1024];
        memset(buf,0,sizeof(buf));
        fflush(stdin);
        fgets(buf,sizeof(buf),stdin);
        buf[strlen(buf)-1]=0;
        write(client_sock,buf,sizeof(buf));

        if(strncasecmp(buf,"quit",4)==0)
        {
            printf("quit!\n");
            break;
        }
        memset(buf,0,sizeof(buf));
        read(client_sock,buf,sizeof(buf));
        printf("server:& %s\n",buf);
    }
}

int main(int argc,char* argv[])
{
    if(argc!=2)
    {
        perror("ip!");
        return 1;
    }
    char* str=argv[1];

    struct sockaddr_in server_socket;
    int sock=socket(AF_INET,SOCK_STREAM,0);
    bzero(&server_socket,sizeof(server_socket));
    server_socket.sin_family=AF_INET;
    inet_pton(AF_INET,str,&server_socket.sin_addr);
    server_socket.sin_port=htons(_PORT);

    int ret=connect(sock,(struct sockaddr*)&server_socket,sizeof(server_socket));
    {
        if(ret<0)
        {
            printf("connect failed... error is: %d ,errstring is: %s\n",errno,strerror(errno));
            return 1;
        }
    }
    printf("connect success!\n");

    serviceFun(sock);
    
    close(sock);
    return 0;
}
