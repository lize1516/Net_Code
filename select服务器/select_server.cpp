#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/select.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>

int startup(char* port)
{
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(atoi(port));
    
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        perror("socket");
        return 2;
    }
    if(bind(sock,(struct sockaddr*)&addr,sizeof(addr))<0)
    {
        perror("bind");
        return 3;
    }
    if(listen(sock,5)<0)
    {
        perror("listen");
        return 4;
    }
    return sock;
}
void Init(int* fd_list,int size)
{   
    int i=0;
    for(i=0;i<size;i++)
    {
        fd_list[i]=-1;
    }
}
void Reload(int listen_fd,int* fd_list,int size,fd_set* read_fds,int* max_fd)
{
    FD_ZERO(read_fds);
    FD_SET(listen_fd,read_fds);
    int max=listen_fd;
    int i=0;
    for(i=0;i<size;i++)
    {
        if(fd_list[i]==-1)
        {
            FD_SET(fd_list[i],read_fds);
            if(fd_list[i]>max)
                max=fd_list[i];
        }
    
    }
    *max_fd=max;
}
void Add(int fd,int* fd_list,int fd_size)
{
    int i=0;
    for(i=0;i<fd_size;i++)
    {
        if(fd_list[i]==-1)
        {
            fd_list[i]=fd;
            break;
        }
    }
    return;
}

int main(int argc,char*argv[])
{
    if(argc!=2)
    {
        perror("Usage: [port]");
        return 1;
    }
    int sock=startup(argv[1]);

    fd_set read_fds;
    int fd_list[1024];
    int fd_size=sizeof(fd_list)/sizeof(int);
    Init(fd_list,fd_size);
    
    for(;;)
    {
        int max_fd=sock;
        Reload(sock,fd_list,fd_size,&read_fds,&max_fd);
        printf("before select:%d\n",FD_ISSET(sock,&read_fds));
        int ret=select(max_fd+1,&read_fds,NULL,NULL,NULL);
        printf("after select:%d\n",FD_ISSET(sock,&read_fds));
        if(ret<0)
        {
            perror("select");
            continue;
        }
        if(ret==0)
        {
            printf("select timeout\n");
            continue;
        }
        //´¦Àílisten_fd
        if(FD_ISSET(sock,&read_fds))
        {
            struct sockaddr_in client_addr;
            socklen_t len=sizeof(client_addr);
            int new_fd=accept(sock,(struct sockaddr*)&client_addr,&len);
            if(new_fd<0)
            {
                perror("accept");
                continue;
            }
            printf("client %s:%d connect\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
            Add(new_fd,fd_list,fd_size);
        }
        int i=0;
         for(i=0;i<fd_size;i++)
         {
             if(fd_list[i]==-1)
              continue;
             if(!FD_ISSET(sock,&read_fds))
              continue;
             char buf[1024]={0};
             int ret=read(fd_list[i],buf,sizeof(buf)-1);
             if(ret<0)
             {
                perror("read");
                continue;
             }
             if(ret==0)
             {
                printf("client quit!\n");
                close(fd_list[i]);
                fd_list[i]=-1;
             }
             printf("client>: %s",buf);
             write(fd_list[i],buf,strlen(buf));
         }
    }
    return 0;
}
