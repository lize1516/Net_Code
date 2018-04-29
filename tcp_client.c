#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>

#define SERVER_PORT 9999
//

int main(int argc,char*argv[])
{
    if(argc!=2)
    {
        perror("usage");
        return 1;
    }
    char*str=argv[1];
    char buf[1024];
    memset(buf,0,sizeof(buf));

    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        perror("socket!");
        return 2;
    }

    struct sockaddr_in server_sock;
    bzero(&server_sock,sizeof(server_sock));
    server_sock.sin_family=AF_INET;
    inet_pton(AF_INET,str,&server_sock.sin_addr);
    server_sock.sin_port=htons(SERVER_PORT);
    
    int ret=connect(sock,(struct sockaddr*)&server_sock,sizeof(server_sock));

    printf("connect success...\n");
///////////////////////////////////////////
    while(1)
    {
        printf("client :#");
        fgets(buf,sizeof(buf),stdin);
        buf[strlen(buf)-1]=0;
        write(sock,buf,sizeof(buf));
        if(strncasecmp(buf,"quit",4)==0)
        {
            printf("quit!\n");
            break;
        }
        read(sock,buf,sizeof(buf));
        printf("server :$ %s\n",buf);
    }
    close(sock);
    return 0;
}
