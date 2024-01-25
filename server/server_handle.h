#ifndef __SERVER_HANDLE_H
#define __SERVER_HANDLE_H

#define MAX_USERNAME  20
#define MAX_PASSWORD  10
#define MAX_CONTENT   1024
#define MAX_TARGET    10



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



/* 信息结构体 */
typedef struct message
{
    int type; //功能选择
    char userName[MAX_USERNAME];//用户名
    char password[MAX_PASSWORD];//密码
    char content[MAX_CONTENT];//发送的内容
    char target[MAX_TARGET];//发送的目标

}message;

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
void * thread_handle(void *arg);


#endif //__SERVER_HANDLE_H