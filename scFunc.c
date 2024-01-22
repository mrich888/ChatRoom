#include "scFunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* 打印操作菜单 */
int print_menu()
{
    printf("1.Login\n");
    printf("2.Register\n");
    printf("3.Direct chat\n");
    printf("4.Group char\n");
    printf("please choose right function\n");
}

/* 客户端登录 */
int client_login(message *msg)
{
    msg->type = Login;
    /* 客户端选择登录的话，需要获取用户名 */
    printf("please input your username:\n");
    fgets(msg->userName, MAX_USERNAME, stdin);
    msg->userName[strcspn(msg->userName, '\n')] = '\0';
    /* 获取密码 */
    printf("please input your username:\n");
    fgets(msg->password, MAX_PASSWORD, stdin);
    msg->password[strcspn(msg->password, '\0')] = '\0';

}

/* 客户端注册  */
int client_register(message *msg)
{
    msg->type = Register;
    /* 获取用户名 */
    printf("please input your username:\n");
    fgets(msg->userName, MAX_USERNAME, stdin);
    msg->userName[strcspn(msg->userName, '\n')] = '\0';
    /* 获取密码 */
    printf("please input your username:\n");
    fgets(msg->password, MAX_PASSWORD, stdin);
    msg->password[strcspn(msg->password, '\0')] = '\0';

}

/* 客户端选择私聊 */
int client_direct_chat(message *msg)
{
    msg->type = Direct_chat;
    /* 获取私聊客户端 */
    printf("please input users who you want to private to: \n");
    fgets(msg->target, MAX_TARGET, stdin);
    msg->target[strcspn(msg->target, '\n')] = '\0';
    /* 获取私聊内容 */
    printf("please input content that you want to send: \n");
    fgets(msg->content, MAX_CONTENT, stdin);
    msg->content[strcspn(msg->content, '\n')] = '\0';

}

/* 客户端选择群聊 */
int client_group_chat(message *msg)
{
    msg->type = Groups_chat;
    /* 获取群聊内容 */
    printf("please input content that you want to send: \n");
    fgets(msg->content, MAX_CONTENT, stdin);
    msg->content[strcspn(msg->content, '\n')] = '\0';

}

/* 客户端发送消息 */
int send_message()
{

}
/* 客户端接收消息 */
int receive_message()
{

}
/* 客户端工作线程 */
void* handle_client(void* arg)
{

}

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
/* 服务器工作线程 */
void* handle_server(void* arg)
{

}
