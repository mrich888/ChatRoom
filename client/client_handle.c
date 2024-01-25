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
void print_menu()
{
    printf("1.Login\n");
    printf("2.Register\n");
    printf("3.Direct chat\n");
    printf("4.Group char\n");
    printf("please choose right function\n");
}

/* 客户端这边需要获取到输入的信息，用json存储
    1.注册/登录：需要一个存储用户信息的json，包括要选择的功能type,用户名username，密码password
    2.客户端将获取到的信息发送给服务器

    服务器

 */




/* 将注册和登录信息发送到服务器 */
int send_to_server(struct json_object *message,  int SERVER_PORT, const char *SERVER_IP)
{
    /* 客户端套接字 */
    int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sockfd  == -1)
    {
        perror("socket error");
        exit(-1);
    }
    /* 绑定要连接的服务器 */
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    int ret = inet_pton(AF_INET, SERVER_IP, (void *)&(serverAddress.sin_addr.s_addr));
    if (ret != 1)
    {
        perror("inet_pton error");
        exit(-1);
    }
    
    /* 绑定之后进行连接 */
    ret = connect(client_sockfd , (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (ret == -1)
    {
        perror("connect error");
        exit(-1);
    }
    /* 连接成功发送信息给服务器 --- 服务器需要对接收的信息进行判断执行什么操作(type中存储的是类型）---判断是否可以登录
    ---接收消息到的消息情况不同
        1. 用户名未注册 ---在数据库中搜索一遍发现未注册  ----返回注册界面
        2. 用户名和密码不匹配 ---在数据库中搜索一遍发现未找到相对应的账号密码 name = "" && password = "";
        3. 匹配成功 ---进行其他功能的选择
     */
    struct json_object * user = message;
    const char *pUser = json_object_get_string(user);
    if (send(client_sockfd, pUser, strlen(*pUser) - 1, 0) < 0)
    {
        perror("send error!");
        close(client_sockfd);
        return 0;
    }
    
    /* 接收服务器发来的消息 */
    
    
   
}



/* 客户端登录 */
int client_login(struct json_object *user, int SERVER_PORT, const char *SERVER_IP)
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
    //struct json_object *userName = json_object_new_string("")
    /* 获取密码 */
    printf("please input your username:\n");
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
    if (type == NULL)
    {
        perror("new type json object error");
        json_object_put();/* do ... while ... */
        exit(-1);
    }
    
    
    /* 将键值对放入到user对象中 */
    int ret = json_object_object_add(user, "userName", userName);
    if(ret < 0)
    {
        perror()
    }
    json_object_object_add(user, "passWord", passWord);
    json_object_object_add(user, "type", type);

    /* 将user对象转换成字符串 */
    //const char *pUser = json_object_to_json_string(user);
    //printf("%s\n", json_object_to_json_string(user));

    /* 将获取到的信息发送给服务器 --- 发送信息 */
    send_to_server(user, SERVER_PORT, SERVER_IP);
    
    /* 用完需要释放 */
    json_object_put(user);
    return 0;

}

/* 客户端注册  */
int client_register(json_object *user, int SERVER_PORT, const char *SERVER_IP)
{
    /* todo... 是否需要全局 */
    struct json_object *user = json_object_new_object();

    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    
    /* 客户端选择登录的话，需要获取用户名密码和功能 */
    printf("please input your username:\n");
    scanf("%s", username);
    //struct json_object *userName = json_object_new_string("")
    /* 获取密码 */
    printf("please input your username:\n");
    scanf("%s", password);

    struct json_object *userName = json_object_new_string(username);
    struct json_object *passWord = json_object_new_string(password);
    struct json_object *type = json_object_new_string("Login");
    
    /* 将获取到的用户名密码放入到user对象中 */
    json_object_object_add(user, "userName", userName);
    json_object_object_add(user, "passWord", passWord);
    json_object_object_add(user, "type", type);

    /* 将user对象转换成字符串 */
    const char *pUser = json_object_to_json_string(user);

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


