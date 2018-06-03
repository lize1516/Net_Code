#include<stdio.h>
#include<unistd.h>
#include<sys/select.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>

void ServiceFun(int fd)
{
    for(;;)
    {
        printf(">");
        fflush(stdout);

        char buf[1024]={0};
        read(0,buf,sizeof(buf)-1);
        int write_size=write(fd,buf,strlen(buf));
        if(write_size<0)
        {
            perror("write");
            continue;
        }
        int read_size=read(fd,buf,sizeof(buf)-1);
        if(read_size<0)
        {
            perror("read");
            continue;
        }
        if(read_size==0)
        {
            printf("server close!\n");
            break;
        }
        printf("server#:%s",buf);
    }
    close(fd);
}
int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        printf("Usage: ./client [ip] [port]");
        return 1;
    }
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(argv[1]);
    addr.sin_port=htons(atoi(argv[2]));

    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd<0)
    {
        perror("socket");
        return 2;
    }
    int ret=connect(fd,(struct sockaddr*)&addr,sizeof(addr));
    if(ret<0)
    {
        perror("connect");
        return 3;
    }
    ServiceFun(fd);
    
    return 0;
}
