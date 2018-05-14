#include<stdio.h>
#include<sys/socket.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define _PORT 8080
#define _BACKLOG 10

int startup()
{
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in socket;
    socket.sin_family=AF_INET;
    socket.sin_addr.s_addr=htonl(INADDR_ANY);
    socket.sin_port=htons(_PORT);

    if(bind(sock,(struct sockaddr*)&socket,sizeof(socket))<0)
    {
        perror("bind");
        close(sock);
        return -2;
    }

    if(listen(sock,_BACKLOG)<0)
    {
        perror("listen");
        close(sock);
        return -3;
    }

    return sock;
}

int main( )
{
    int sock=startup();
    if(sock<0)
    {
        return -1;
    }

    for(;;)
    {
        struct sockaddr_in client;
        socklen_t len=0;
        int client_sock=accept(sock,(struct sockaddr*)&client,&len);
        if(client_sock<0)
        {
            perror("accept");
            continue;
        }
        for(;;)
        {
            char buf_input[1024*10];
            memset(buf_input,0,sizeof(buf_input));
            size_t read_size=read(client_sock,buf_input,sizeof(buf_input)-1);
            if(read_size<0)
            {
                return 1;
            }
            if(read_size==0)
            {
                break;
            }
            printf("[Request] %s\n",buf_input);
            char buf[1024];
            const char* hello="<h1>Hello world</h1>";
            sprintf(buf,"http/1.0 200 OK\nContent-Length:%lu\n%s",strlen(hello),hello);
            write(client_sock,buf,strlen(buf));
        }
    }
    close(sock);
    return 0;
}

