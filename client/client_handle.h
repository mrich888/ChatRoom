#ifndef __SC_FUNC_H_
#define __SC_FUNC_H_

#define MAX_TYPE      20
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


/* 打印操作菜单 */
void print_menu();
/* 客户端登录 */
int client_login(int sockfd, struct json_object *message);
/* 客户端注册  */
int client_register(int sockfd, struct json_object *message);
/* 发送注册和登录信息到服务器 */
int send_to_server(int sockfd, struct json_object *message);
/* todo...看是否可以跟后面的发送合在一起 */
/* 客户端选择私聊 */
int client_direct_chat();
/* 客户端选择群聊 */
int client_group_chat();
/* 客户端发送消息 */
int send_message();
/* 客户端接收消息 */
int receive_message();
/* 客户端工作线程 */
void* handle_client(void* arg);




#endif //__SC_FUNC_H_