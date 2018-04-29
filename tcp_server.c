#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>

#define _PORT 9999
#define _BACKLOG 10     //

int main()
{
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        perror("socket!");
        return 2;
    }

    struct sockaddr_in server_sock;
    struct sockaddr_in client_sock;
    bzero(&server_sock,sizeof(server_sock));
    server_sock.sin_family=AF_INET;
    server_sock.sin_addr.s_addr=htonl(INADDR_ANY);
    server_sock.sin_port=htons(_PORT);

    if(bind(sock,(struct sockaddr*)&server_sock,sizeof(struct sockaddr_in))<0)
    {
        perror("bind!");
        close(sock);
        return 3;
    }

    if(listen(sock,_BACKLOG)<0)
    {
        perror("listen!");
        close(sock);
        return 4;   
    }
    
    printf("bind and listen success,wait accept...\n");
///////////////////////////////////////////
    while(1)
    {
        socklen_t len=0;
        int newsock=accept(sock,(struct sockaddr*)&client_sock,&len);
        if(newsock<0)
        {
            perror("accept!");
            close(sock);
            return 5;
        }
        char buf_ip[INET_ADDRSTRLEN];
        memset(buf_ip,0,sizeof(buf_ip));
        inet_ntop(AF_INET,&client_sock.sin_addr,buf_ip,sizeof(buf_ip));
        
        printf("get connect,ip is:%s port is:%d\n",buf_ip,ntohs(client_sock.sin_port));

        while(1)
        {
            char buf[1024];

            int s=read(newsock,buf,sizeof(buf)-1);
            if(s>1)
            {
                buf[s]=0;
                printf("client:# %s\n",buf);
                write(newsock,buf,strlen(buf));     
            }
            else if(s==0)
            {
                printf("client [ip:%s | %d] exit!\n",buf_ip,ntohs(client_sock.sin_port));
                break;
            }

        }
    }
    close(sock);
    return 0;
}
