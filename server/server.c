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

/* 线程处理函数:传过来一个通信句柄，一个json对象的字符串 */
void * thread_handle(void* args)
{
    /* 设置线程分离 */
    pthread_detach(pthread_self());
    /* 获取到通信句柄. */
    int sockfd = *(int *)args;

    /* todo....这里需要完善一下在线列表 */

    /* 定义一个json对象:接收缓存区 */
    //struct json_object *user = json_object_new_object();
    /* 读缓存区 */
    char recvBuffer[BUFFER_SIZE];
    memset(recvBuffer, 0, sizeof(recvBuffer));
    /* 先接收消息再解析传过来的json时有多种情况：登录 / 注册 / 私聊 / 群聊  */
    while (recv(sockfd, recvBuffer, sizeof(recvBuffer), 0) > 0)
    {
        /* 将获取的字符串转成json对象 */
        struct json_object *user = json_tokener_parse(recvBuffer); 
        /* 解析json,获取功能 */
        struct json_object *typeVal = json_object_object_get(user, "type");
        /* 将type转换成字符串 */
        const char * pType = json_object_get_string(typeVal);
        /* 比较两个字符串,实现相应的功能 */
        if (strcmp(*pType, "Login") == 0)
        {
            handle_login(sockfd, user);
        }
        else if(strcmp(*pType, "Register") == 0)
        {
            handle_register(sockfd, user);
        }
        else if (strcmp(*pType, "Direct_chat") == 0)
        {
            handle_direct_message();
        }
        else if (strcmp(*pType, "Groups_chat") == 0)
        {
            handle_group_chat();
        }
        else
        {
            printf("There is currently no functionality for this feature.....");
        }
    }
    /* 线程退出 */
    pthread_exit(NULL);
}


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