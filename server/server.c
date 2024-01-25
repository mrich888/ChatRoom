#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <error.h>
#include <pthread.h>
#include "server_handle.h"
#include "threadPool.h"


#define SERVER_PORT 8080
#define MAX_LISTEN  128
#define LOCAL_IPADDRESS "127.0.0.1"
#define BUFFER_SIZE 128


#define MINTHREADS      5
#define MAXTHREADS      10
#define MAXQUEUESIZE    50



void sigHander(int sigNum)
{
    printf("ignore...\n");

    return;
}


int main()
{
#if 1
    /* 初始化线程池 */
    threadpool_t pool;
    threadPoolInit(&pool, MINTHREADS, MAXTHREADS, MAXQUEUESIZE);
#endif

    /* 信号注册 */
    // signal(SIGINT, sigHander);
    // signal(SIGQUIT, sigHander);
    // signal(SIGTSTP, sigHander);

    /* 信号注册 */
    signal(SIGPIPE, sigHander);



    /* 创建socket套接字 */
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

    /* 绑定 */
    struct sockaddr_in localAddress;
    /* 清除脏数据 */
    memset(&localAddress, 0, sizeof(localAddress));

    /* 地址族 */
    localAddress.sin_family = AF_INET;
    /* 端口需要转成大端 */
    localAddress.sin_port = htons(SERVER_PORT);
    /* ip地址需要转成大端 */

    /* Address to accept any incoming messages.  */
    /* INADDR_ANY = 0x00000000 */
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY); 

    
    int localAddressLen = sizeof(localAddress);
    ret = bind(sockfd, (struct sockaddr *)&localAddress, localAddressLen);
    if (ret == -1)
    {
        perror("bind error");
        exit(-1);
    }

    /* 监听 */
    ret = listen(sockfd, MAX_LISTEN);
    if (ret == -1)
    {
        perror("listen error");
        exit(-1);
    }

    /* 客户的信息 */
    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));

    
    /* 循环去接收客户端的请求 */
    while (1)
    {
        socklen_t clientAddressLen = 0;
        /* 局部变量到下一次循环地方就会被释放. */
        int acceptfd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientAddressLen);
        if (acceptfd == -1)
        {
            perror("accpet error");
            exit(-1);
        }
        /* 将任务添加任务队列 */
        threadPoolAddTask(&pool, thread_handle, (void *)&acceptfd);
    } 

#if 1
    /* 释放线程池 */
    threadPoolDestroy(&pool);
#endif


    /* 关闭文件描述符 */
    close(sockfd);

    return 0;
}