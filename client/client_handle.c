#include "client_handle.h"
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
#include <json-c/json.h>

#define BUFFER_SIZE 128

/* 打印操作菜单 */
void print_menu1()
{
    printf("1.Login\n");
    printf("2.Register\n");
    printf("3.Direct chat\n");
    printf("4.Group char\n");
    printf("please choose right function\n");
}

/* 打印操作菜单 */
void print_menu2()
{
    printf("1.Direct chat\n");
    printf("2.Group char\n");
    printf("please choose right function\n");
}

/* 登录之后的页面 */
int login_after()
{
    /* 跳转到主页面，继续选择功能 */
    int choice = 0;
    login_after();
    scanf("%d", &choice);
    getchar();
    switch (choice)
    {
    case Direct_chat:
        handle_direct_message();
        break;
    case Groups_chat:
        handle_group_chat();
        break;
    default:
        printf("Please select the correct function!");
        break;
    }
}


/* 将注册和登录信息发送到服务器
    如何确定发送给谁呢？通过套接字确定发送给谁---获取服务器的套接字；
    如何确定发送什么东西呢？通过json封装信息 ---传json对象
*/
/* 发送注册和登录信息到服务器 */
int send_to_server(int sockfd, struct json_object *message)
{
    /* 拿到json对象 */
    struct json_object * user = message;
    /* 将json解析成字符串 */
    const char *pUser = json_object_get_string(user);
    /* 发送字符串信息给服务器 */
    if (send(sockfd, pUser, strlen(pUser) - 1, 0) < 0)
    {
        perror("send error!");
        close(sockfd);
        return 0;
    }

    /* 接收服务器发来的信息 --- 成功 / 注册失败 / 登录失败 .... */
    char recvBuffer[BUFFER_SIZE];
    memset(recvBuffer, 0, sizeof(recvBuffer));
    recv(sockfd, recvBuffer, sizeof(recvBuffer), 0);
    if (strcmp(recvBuffer, "Welcome to visit!") == 0)
    {
        /* 登录成功跳转 */
        login_after();
    }
    if (strcmp(recvBuffer, "Already register!") == 0)
    {
        /* 密码错误，重新跳转到最开始去获取用户名密码 --- 跳转到初始化界面吧。。。。 */
        
    }

    return 0;
}

/* 客户端这边需要获取到输入的信息并发送给服务器
    1.注册/登录：需要一个存储用户信息的json，包括要选择的功能type,用户名username，密码password
    2.客户端将获取到的信息发送给服务器 --- 发送的时候带上通信句柄，让服务器知道是谁发的
*/
/* 客户端登录 */
int client_login(int sockfd, json_object *user)
{
    /* 存放信息的用户对象 */
    //struct json_object *user = json_object_new_object();
    if (user == NULL)
    {
        perror("new user json object error");
        exit(-1);
    }
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    /* 客户端选择登录的话，需要获取用户名密码和功能 */
    printf("please input your username:\n");
    scanf("%s", username);
    printf("please input your password:\n");
    scanf("%s", password);

    /* 创建用户名和密码键值对 */
    struct json_object *userName = json_object_new_string(username);
    if (userName == NULL)
    {
        perror("new userName json object error");
        exit(-1);
    }
    struct json_object *passWord = json_object_new_string(password);
    if (passWord == NULL)
    {
        perror("new passWord json object error");
        exit(-1);
    }
    
    struct json_object *type = json_object_new_string("Login");
    // if (type == NULL)
    // {
    //     perror("new type json object error");
    //     json_object_put();/* do ... while ... */
    //     exit(-1);
    // }
    
    /* 将键值对放入到user对象中 */
    int ret = json_object_object_add(user, "userName", userName);
    // if(ret < 0)
    // {
    //     perror()
    // }
    json_object_object_add(user, "passWord", passWord);
    json_object_object_add(user, "type", type);

    /* 将获取到的json对象发送给服务器 --- 发送信息 */
    send_to_server(sockfd, user);
    
    /* 用完需要释放 */
    json_object_put(user);

    return 0;
}

/* 客户端注册  */
int client_register(int sockfd, json_object *user)
{
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    
    /* 客户端选择注册的话，需要将获取用户名密码和功能发送给服务器，服务器将信息存储在数据库中 */
    printf("please input your username:\n");
    scanf("%s", username);
    printf("please input your password:\n");
    scanf("%s", password);

    struct json_object *userName = json_object_new_string(username);
    struct json_object *passWord = json_object_new_string(password);
    struct json_object *type = json_object_new_string("Register");
    
    /* 将获取到的用户名密码放入到user对象中 */
    json_object_object_add(user, "userName", userName);
    json_object_object_add(user, "passWord", passWord);
    json_object_object_add(user, "type", type);

    /* 将获取到的json对象发送给服务器 --- 发送信息 */
    send_to_server(sockfd, user);

    /* 用完需要释放 */
    json_object_put(user);
    return 0;

}

/* 客户端选择私聊 */
int client_direct_chat()
{

}

/* 客户端选择群聊 */
int client_group_chat()
{
   

}

/* 客户端发送消息 */
int send_message()
{
    /**/

}
/* 客户端接收消息 */
int receive_message()
{

}
/* 客户端工作线程 */
void* handle_client(void* arg)
{

}


