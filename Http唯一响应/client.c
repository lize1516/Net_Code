#include<stdio.h>
#include<sys/socket.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

int startup(char* addr,char* port)
{
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in socket;
    socket.sin_family=AF_INET;
    socket.sin_addr.s_addr=inet_addr(addr);
    socket.sin_port=htons(atoi(port));
    
    int ret=connect(sock,(struct sockaddr*)&socket,sizeof(socket));
    if(ret<0)
    {
        perror("connect");
        return -2;
    }

    return sock;
}

int main(int argc,char*argv[])
{
    if(argc!=3)
    {
        perror("Useage");
        return -1;
    }
    int sock=startup(argv[1],argv[2]);
    if(sock<0)
    {
        return -1;
    }

    for(;;)
    {
        char buf[1024*10];
        memset(buf,0,sizeof(buf));
        printf("client#:");
        fgets(buf,sizeof(buf),stdin);
        buf[strlen(buf)-1]=0;
        write(sock,buf,strlen(buf));

        printf("server$:");
        memset(buf,0,sizeof(buf));
        int size=read(sock,buf,sizeof(buf));

        printf("%s\n",buf);
    }
    close(sock);
    return 0;
}

