#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
 
#define IP "127.0.0.1"
#define PORT 7777
#define BUFFER_SIZE 1024

int main()
{
    int client_socket = 0, ret = 0, len = 0;
    char buf[BUFFER_SIZE] = {0};
    struct sockaddr_in client_sockaddr;
    len = sizeof(client_sockaddr);
    //建立监听套接字
    client_socket = socket(AF_INET,SOCK_STREAM,0);
 
    //绑定IP地址和端口号
    client_sockaddr.sin_family = AF_INET;
    client_sockaddr.sin_port = htons(PORT);
    client_sockaddr.sin_addr.s_addr = inet_addr(IP);
    //建立链接
    ret = connect(client_socket,(struct sockaddr *)&client_sockaddr, len);
    if(ret == -1)
    {
        perror("connect");
        return -1;
    }
    printf("链接成功！\n");

    while(1)
    {
        //发送
        memset(buf, 0, BUFFER_SIZE);
        printf("发送的数据>>\n");
        fgets(buf, BUFFER_SIZE, stdin);
        //send(client_socket,buf,strlen(buf),0);
        write(client_socket, buf, strlen(buf));
        //接受数据
        memset(buf,0,BUFFER_SIZE);
        //recv(client_socket,buf,BUFFER_SIZE,0);
        read(client_socket,buf,BUFFER_SIZE);
        printf("收到的数据>>%s\n",buf);
        
    }
 
    close(client_socket);
}