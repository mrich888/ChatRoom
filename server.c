#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include "threadPool.h"

#define MAX_LISTEN 128
#define BUFFER_SIZE 128
#define SERVER_PORT 8080
#define LOCAL_IPADDRESS "127.0.0.1"

/* 线程执行的任务 */
void * threadHandle(void *arg)
{
    /* 设置线程分离 */
    pthread_detach(pthread_self());
    /* 通信句柄 */
    int acceptfd = *(int *)arg;

    /* 开始通信 */
    /* 接收缓冲区 */
    char recvBuffer[BUFFER_SIZE];
    memset(recvBuffer, 0, sizeof(recvBuffer));

    /* 发送缓冲区 */
    char sendBuffer[BUFFER_SIZE];
    memset(sendBuffer, 0, sizeof(sendBuffer));

    /* 读取到的字节数 */
    int readBytes = 0;
    while (1)
    {
        readBytes = read(acceptfd, recvBuffer, sizeof(recvBuffer));
        if (readBytes <= 0)
        {
            perror("read error");
            close(acceptfd);
            break;
        }
        else
        {
            if (recvBuffer == "注册")
            {

            }
            else if (recvBuffer == "登陆")
            {

            }

        }
    }

    pthread_exit(NULL);
    

}

int main()
{
    /* 先创建套接字 */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(-1);
    }

    /* 设置端口复用 */
    int enableOpt = 1;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&enableOpt, sizeof(enableOpt));
    if (ret == -1)
    {
        perror("setsockopt error");
        exit(-1);
    }

    /* 开始绑定 */
    struct sockaddr_in localAddress;
    memset(&localAddress, 0, sizeof(localAddress));

    /* 分配地址族 */
    localAddress.sin_family = AF_INET;
    /* 端口和ip都需要转换成大端 */
    localAddress.sin_port = htons(SERVER_PORT);
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    int localAddressLen = sizeof(localAddress);
    int ret = bind(sockfd, (struct sockaddr *)&localAddress, localAddressLen);
    if (ret == -1)
    {
        perror("bind error");
        exit(-1);
    }

    /* 开始监听 */
    ret = listen(sockfd, MAX_LISTEN);
    if (ret == -1)
    {
        perror("listen error");
        exit(-1);
    }

    /* 客户的信息 */
    struct sockaddr_in clientAdress;
    memset(&clientAdress, 0, sizeof(clientAdress));

    while (1)
    {
        socklen_t clientAdressLen = 0;
        /* 接收信息 */
        int acceptfd = accept(sockfd, (struct sockaddr *)&clientAdress, &clientAdressLen);
        if (acceptfd == -1)
        {
            perror("accept error");
            exit(-1);
        }

        /* 将任务添加到任务队列 */
        
    }

    close(sockfd);



    return 0;
}