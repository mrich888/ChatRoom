#ifndef __SERVER_HANDLE_H
#define __SERVER_HANDLE_H

#include <json-c/json.h>
#include <sqlite3.h>

#define MAX_USERNAME  20
#define MAX_PASSWORD  10
#define MAX_CONTENT   1024
#define MAX_TARGET    10

/* 创建一个数据库 */
sqlite3 *chatRoom;
/* 初始化一个数据库的锁 */
pthread_mutex_t mutex_db = PTHREAD_MUTEX_INITIALIZER;

enum CHOICE 
{
    /* 登录 */
    Login = 1,
    /* 注册 */
    Register,
    /* 私聊 */
    Direct_chat,
    /* 群聊 */
    Groups_chat,
};

/* 在线客户端 */
typedef struct client
{
    int socket;
    char userName[MAX_USERNAME];
}client;


/* 服务器执行登录操作 */
int handle_login();
/* 服务器执行注册操作 */
int handle_register();
/* 服务器执行私聊操作 */
int handle_direct_message();
/* 服务器执行群聊操作 */
int handle_group_chat();
/* 线程处理函数 */
void * thread_handle(void* args);


#endif //__SERVER_HANDLE_H