#include "server_handle.h"
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
#include <unistd.h>
#include <json-c/json.h>


#define BUFFER_SIZE 1028

/* 全局锁 */
pthread_mutex_t mutex_db = PTHREAD_MUTEX_INITIALIZER;

/* 服务器执行登录操作 */
int handle_login()
{
    
}
/* 服务器执行注册操作 */
int handle_register()
{

}
/* 服务器执行私聊操作 */
int handle_direct_message()
{

}
/* 服务器执行群聊操作 */
int handle_group_chat()
{

}
/* 线程处理函数:传过来一个通信句柄，一个json对象的字符串 */
void * thread_handle(int sockfd, const char * message)
{
    /* 设置线程分离 */
    pthread_detach(pthread_self());
    /* 获取到通信句柄. */
    int client_sockfd = sockfd;
    char *pUser = message;
    /* 将字符串转化为json */
    struct json_object *user = json_tokener_parse(pUser);
    /* 解析json,获取数据类型 */
    struct json_object *type = json_object_object_get(user, "type");

    /* 接收消息有多种情况 */
   
    while (recv(client_sockfd, type, BUFFER_SIZE, 0) > 0)
    {
        switch (*type)
        {
        case Login:
            /* code */
            break;
        
        default:
            break;
        }
    }
    


    /* 线程退出 */
    pthread_exit(NULL);
}
