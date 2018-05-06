#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/wait.h>

#define _PORT 8080
#define _BACKLOG 10

int create_Sock(struct sockaddr_in* server_sock)
{
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        perror("socket");
        return -1;
    }

    bzero(server_sock,sizeof(*server_sock));
    server_sock->sin_family=AF_INET;
    server_sock->sin_addr.s_addr=htonl(INADDR_ANY);
    server_sock->sin_port=htons(_PORT);

    if(bind(sock,(struct sockaddr*)server_sock,sizeof(struct sockaddr_in))<0)
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
    printf("bind and listen success,wait accept...\n");
    return sock;
}

void servicefun(int client_sock,char*buf_ip)
{
    while(1)
    {
        char buf[1024];
        memset(buf,0,sizeof(buf));

        read(client_sock,buf,sizeof(buf)-1);
        if(strcmp(buf,"")==0)
        {
            continue;
        }
        if(strcmp(buf,"quit")==0)
        {
            printf("client[ip:%s],exit!\n",buf_ip);
            break;
        }
        printf("client[ip:%s]#: %s\n",buf_ip,buf);
        
       memset(buf,0,sizeof(buf));
       printf("server$: ");
       fgets(buf,sizeof(buf),stdin);
       buf[strlen(buf)-1]=0;

        write(client_sock,buf,strlen(buf));
    }

}
void CreateWorker(int client_sock,char* buf_ip)
{
    pid_t pid=fork();
    if(pid<0)
    {
        perror("fork");
        return;
    }
    else if(pid==0)
    {
        //child
        if(fork()==0)
        {
            servicefun(client_sock,buf_ip);
        }
        exit(0);
    }
    else
    {
        //father

        close(client_sock);
        waitpid(pid,NULL,0);
    }
}

int main()
{

    struct sockaddr_in client_socket;
    struct sockaddr_in server_socket;
    int sock=create_Sock(&server_socket);

    while(1)
    {
        socklen_t len=0;
        int client_sock=accept(sock,(struct sockaddr*)&client_socket,&len);
        if(client_sock<0)
        {
            perror("accept");
            close(sock);
            return -4;
        }
        char ip_buf[INET_ADDRSTRLEN];
        memset(ip_buf,0,sizeof(ip_buf));
        inet_ntop(AF_INET,&client_socket.sin_addr,ip_buf,sizeof(ip_buf));

        printf("get connect, ip is:%s port:%d\n",ip_buf,ntohs(client_socket.sin_port));
        CreateWorker(client_sock,ip_buf);
    
    }

    close(sock);
    return 0;
}
