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

/* 连接成功发送信息给服务器 --- 服务器需要对接收的信息进行判断执行什么操作(解析type中存储的是类型）---登录 / 注册
    ---接收消息到的消息情况不同
        1. 用户名未注册 ---在数据库中搜索一遍发现未注册  ----返回注册界面 --- 注册完返回登录界面
        2. 用户名和密码不匹配 ---在数据库中搜索一遍发现未找到相对应的账号密码 name = "" && password = "" --- 重新输入
        3. 匹配成功 ---跳转到主页面 ---进行其他功能的选择
*/
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
void * thread_handle(int sockfd, struct json_object *message)
{
    /* 设置线程分离 */
    pthread_detach(pthread_self());
    /* 获取到通信句柄. */
    int client_sockfd = sockfd;
    /* 获取到json对象 */
    struct json_object *user = message;
    /* 解析json,获取功能 */
    struct json_object *type = json_object_object_get(user, "type");
    /* 将获取到的type转换成 */
    const char * pType = json_object_get_string(type);
    /* 接收消息并解析传过来的json时有多种情况：登录 / 注册 / 私聊 / 群聊  */
    while (recv(client_sockfd, pType, BUFFER_SIZE, 0) > 0)
    {
        if (strcmp(*pType, "Login") == 0)
        {
            handle_login();
        }
        else if(strcmp(*pType, "Register") == 0)
        {
            handle_register();
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
