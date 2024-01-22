#ifndef __SC_FUNC_H_
#define __SC_FUNC_H_

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

/* 打印操作菜单 */
int print_menu();
/* 客户端登录 */
int client_login(message *msg);
/* 客户端注册  */
int client_register(message *msg);
/* 客户端选择私聊 */
int client_direct_chat(message *msg);
/* 客户端选择群聊 */
int client_group_chat(message *msg);
/* 客户端发送消息 */
int send_message();
/* 客户端接收消息 */
int receive_message();
/* 客户端工作线程 */
void* handle_client(void* arg);

/* 服务器执行登录操作 */
int handle_login();
/* 服务器执行注册操作 */
int handle_register();
/* 服务器执行私聊操作 */
int handle_direct_message();
/* 服务器执行群聊操作 */
int handle_group_chat();
/* 服务器工作线程 */
void* handle_server(void* arg);


#endif //__SC_FUNC_H_