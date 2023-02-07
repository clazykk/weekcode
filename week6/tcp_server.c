#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
 
#define IP "127.0.0.1"
#define PORT 7777
#define BUFFER_SIZE 1024

/* void tcp_thread(void * cconnfd)
{
    int connfd =*(int *)cconnfd;
    int recv_len,send_len;
    char buff[100];
    
    while(1)
    {
        recv_len = recv(connfd,buff,100,0);
        if(recv_len == 0)
        {
            break;
        }
        buff[recv_len] = '\0';
        printf("received from client%d the following:\n",connfd);
        printf("%s",buff);


        for(int i=0; i<recv_len; i++)
        {
            buff[i] = toupper(buff[i]);
        }
        send_len = send(connfd,buff,recv_len,0);
        printf("have replied client%d ,over. \n ",connfd);
        printf("------------------------------ \n");
    }
    pthread_exit(NULL);
} */
 
int main()
{
    int server_socket = 0, client_socket = 0, ret = 0, len = 0;
    char buf[BUFFER_SIZE] = {0};
    struct sockaddr_in server_sockaddr, client_sockaddr;
    //建立监听套接字
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    //绑定IP地址和端口号
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(PORT);
    server_sockaddr.sin_addr.s_addr = inet_addr(IP);
    ret = bind(server_socket, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr));
    if(ret == -1)
    {
        perror("bind");
        return -1;
    }
    printf("绑定成功！\n");
    //开始监听
    ret = listen(server_socket, 2);
    if(ret == -1)
    {
        perror("listen");
        return -1;
        
    }
    printf("监听成功！\n");

    fd_set rdset, tmp;
    FD_ZERO(&rdset);
    FD_SET(server_socket, &rdset);
    int maxfd = server_socket;
    while(1) {
        tmp = rdset; //rdset这个不能变，因为内核再检测时，如果没有数据，就会将其变为0，因此，我们需要复制一份。

        // 调用select系统函数，让内核检测哪些文件描述符有数据
        int ret = select(maxfd + 1, &tmp, NULL, NULL, NULL);
        if(ret == -1) {
            perror("select");
            exit(-1);
        } else if(ret == 0) { //设置了永久阻塞NULL，不可能为0
            continue;   
        } else if(ret > 0) { //ret只会返回文件描述符发生变化的个数，不知道具体哪个发生了变化，需要遍历查找
            // 说明检测到了有文件描述符的对应的缓冲区的数据发生了改变
            if(FD_ISSET(server_socket, &tmp)) {
                // 有新的客户端连接
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(server_socket, (struct sockaddr *)&cliaddr, &len);
                printf("与%d链接成功\n", cfd);
                // 将新的文件描述符加入到集合中，下一次select检测时，需要检测这些通信的文件描述符有没有数据
                FD_SET(cfd, &rdset);
                // 更新最大的文件描述符
                maxfd = maxfd > cfd ? maxfd : cfd;
            }
            //检测剩余文件描述符有没有数据变化，从server_socket+1开始即可
            for(int i = server_socket + 1; i <= maxfd; i++) {
                if(FD_ISSET(i, &tmp)) {
                    // 说明这个文件描述符对应的客户端发来了数据
                    int len = read(i, buf, sizeof(buf));
                    if(len == -1) {
                        perror("read");
                        exit(-1);
                    } else if(len == 0) {   //客户端断开连接
                        printf("client %d closed...\n", i);
                        close(i);   //关闭文件描述符
                        FD_CLR(i, &rdset);  //fd_set中不再监测这个文件描述符
                    } else if(len > 0) {
                        printf("read buf = %s\n", buf);
                        write(i, buf, strlen(buf) + 1);
                    }
                }
            }
        }
    }
 
    /* //等待链接
    len = sizeof(client_sockaddr);
    client_socket = accept(server_socket,(struct sockaddr *)&client_sockaddr,&len);
    printf("与%s链接成功\n", inet_ntoa(client_sockaddr.sin_addr));
    
    while(1)
    {
        //接受数据
        memset(buf, 0, BUFFER_SIZE);
        recv(client_socket, buf, BUFFER_SIZE, 0);
        printf("收到的数据>>%s\n", buf);
        //发送
        memset(buf, 0, BUFFER_SIZE);
        printf("发送的数据>>\n");
        fgets(buf, BUFFER_SIZE, stdin);
        send(client_socket, buf, strlen(buf), 0);
    } */

    close(server_socket);
}
