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
    登录时接收到的消息情况不同
        1. 用户名未注册 ---在数据库中搜索一遍发现未注册  ----返回注册界面 --- 注册完返回登录界面
        2. 用户名和密码不匹配 ---在数据库中搜索一遍发现未找到相对应的账号密码 name = "" && password = "" --- 重新输入
        3. 匹配成功 ---跳转到主页面 ---进行其他功能的选择
*/
/*
    登录成功后需要将该客户端加入到在线队列中 --- 服务器需要维护一个客户端在线列表 --- json封装 
    在线队列：用户名 + socke
*/

/* 服务器执行登录操作 */ 
int handle_login(int sockfd, struct json_object *message)
{
    /* ⭐查询时需要加锁 ⭐json的资源需要释放（do .... while ....） */
    int sockfd = sockfd;
    struct json_object * user = message;
    /* 登录需要获取到用户名和密码和类型,所以需要将获取到的json对象通过键获取到相应的值 */
    struct json_object * userNameVal = json_object_object_get(user, "userName"); //username
    struct json_object * passWordVal = json_object_object_get(user, "passWord"); //password
    struct json_object * typeVal = json_object_object_get(user, "passWord"); //type
    /* 将获取到的值转换成字符串 */
    const char *pUserName =json_object_to_json_string(userNameVal);
    const char *pPassWord =json_object_to_json_string(passWordVal);
    const char *type =json_object_to_json_string(typeVal);
    /* 接下来需要将获取到的用户名和密码和数据库中的信息做比对 */
    char select_sql[BUFFER_SIZE];
    memset(select_sql, 0, sizeof(select_sql));
    /* 查询语句 */
    sprintf(select_sql, "select * from where username = '%s' and password = '%s'", pUserName, pPassWord);
    /* 查询并获得结果 ---get table */
    int row = 0;
    int column = 0;
    char **result = NULL;
    char * errormsg = NULL;//存放数据库出错类型
    int ret = sqlite3_get_table(chatRoom, select_sql, &result, &row, &column, &errormsg);
    if (ret != SQLITE_OK)
    {
        printf("select error:%s\n", errormsg);
        exit(-1);
    }
    /* 查询并获得结果，判断是什么情况，要执行什么操作 
        1.查询到了 --- 行列都有值/有符合的行和列并且能打印出来 ---
        2.未查询到 --- 没有查询到  --- 也不一定是没有注册，还有可能是不匹配  
        3.再去查用户名 --- 用户名存在就是密码不匹配，重新输入 ---查不到的话就到注册界面
    */
    char response[BUFFER_SIZE];
    memset(response, 0, sizeof(response));
    if (row > 0)
    {
        /* 有符合的行就证明匹配成功,给客户端发送登陆成功信息 */
        //printf("Welcome to visit!");
        strncpy(response, "Welcome to visit!", sizeof(response) - 1);
        send(sockfd, response, sizeof(response), 0);
        
    }
    else/* 如果没有则继续判断是否注册 --- 查询用户名是否存在 --- 用户名作主码*/
    {
        /* 查询语句  ---  todo....可以封装成一个函数   */
        sprintf(select_sql, "select * from where username = '%s'", pUserName);
        /* 查询并获得结果 ---get table */
        ret = sqlite3_get_table(chatRoom, select_sql, &result, &row, &column, &errormsg);
        if (ret != SQLITE_OK)
        {
            printf("select error:%s\n", errormsg);
            exit(-1);
        }
        if (row > 0) //已注册 --- 不匹配 --- 重新输入并传输
        {
            strncpy(response, "Already register!", sizeof(response) - 1);
            send(sockfd, response, sizeof(response), 0);
        }
        else //未注册 --- 直接跳转到注册界面
        {
            handle_register(sockfd, user);               
        }
    }
    
}
/* 服务器执行注册操作 */
int handle_register(int client_sockfd, struct json_object *message)
{
    /* 将用户的信息转过来 */
    struct json_object *user = message;
    /* 直接将json对象转换成string类型 直接将string 和数据库中的数据进行对比和插入 数据库不能插入json类型的数据 */
    /* 将json中的name取出来(总而言之就是将json对象里面的username专门拿出来) 进行搜索对比 */
    struct json_object * nameVal = json_object_object_get(user, "userName");
    if (nameVal == NULL)
    {
        perror("get nameVal error");
        exit(-1);
    }
    char *nameValue = json_object_get_string(nameVal); 

    /* 将密码用string取出来 */
    struct json_object * pwdVal = json_object_object_get(user, "passWord");
    if (pwdVal == NULL)
    {
        perror("get pwdVal error");
        exit(-1);
    }
    char * pwdValue = json_object_get_string(&pwdVal);


    /* 打开对应数据库的用户表 */
    sqlite3 *mydb = NULL;
    int ret = sqlite3_open("chatRoom.db", &mydb);
    if (ret != SQLITE_OK)
    {
        perror("open error");
        exit(-1);
    }
    /* 加锁 定义一个锁 */
    pthread_mutex_lock(&mutex_db);

    /* 打开数据成功后 要将用户注册输入的名字与数据库中的数据进行对比
        如果相等 就说明有重名的就需要用户重新输入一个名称          */
    char sqlBuffer[BUFFER_SIZE];
    memset(sqlBuffer, 0, sizeof(sqlBuffer));
    sprintf(sqlBuffer, "select * from chatRoom where username = '%s'", &nameValue);

    char **result = NULL;
    int row = 0, column = 0;
    ret = sqlite3_get_table(mydb, sqlBuffer, &result, &row, &column, NULL);
    if (ret == SQLITE_OK)
    {
        
        if (row > 0)
        {
            /* 程序运行到这里就说明用户注册的Name在该数据库中找到了
                在数据库中 有数据的是第一行 所以如果row大于0的时候 说明一定在该数据库中找到了这个用户名 */
            send(client_sockfd, "username already exists, please put anther username!", sizeof("username already exists, please put anther username!"), 0);
        }
        else
        {
            /* 程序到这就是没有找到该用户名 要将username和password存储进数据库中 */
            sprintf(sqlBuffer, "insert into user(username, password)values'%s', '%s", &nameValue, &pwdVal);
            ret = sqlite3_exec(mydb, sqlBuffer, NULL, NULL, NULL);
            if (ret == SQLITE_OK)
            {
                /* 说明执行没有出错 注册成功 */
                send(client_sockfd, "Register successful! please Login", sizeof("Register successful! please Login"), 0);
                /* 这边可以考虑直接跳到登录函数 */
                handle_login(client_sockfd, user);
            }
            else
            {
                /* 说明语句或者哪里出错 没有注册成功 */
                send(client_sockfd, "Register failure", sizeof("Register failure"), 0);
            }
        }
        sqlite3_free_table(result);
    }
    /* 解锁 释放锁操作 */
    pthread_mutex_unlock(&mutex_db);

}
#if 1
/* 服务器执行私聊操作 */
int handle_direct_message(int client_sockfd, struct json_object *message)
{
    /* 获取私聊信息 */
    struct json_object *directMessage = message;
    struct json_object *senderNameVal = json_object_object_get(directMessage, "senderName");
    struct json_object *receiverNameVal = json_object_object_get(directMessage, "receiverName");
    struct json_object *messageVal = json_object_object_get(directMessage, "message");

    /* 将私聊信息转换为字符串 */
    const char *senderName = json_object_to_json_string(senderNameVal);
    const char *receiverName = json_object_to_json_string(receiverNameVal);
    const char *message = json_object_to_json_string(messageVal);

    /* 构建私聊消息 */
    char directMessage[BUFFER_SIZE];
    snprintf(directMessage, sizeof(directMessage), "[Private] %s: %s", senderName, message);

    /* 查找接收者的套接字 */
    int receiverSocket = -1;
    // pthread_mutex_lock(&mutex_db);
    // for (int idx = 0; idx < online_clients.size(); idx++) {
    //     if (strcmp(client_names[online_clients[idx]], receiverName) == 0) {
    //         receiverSocket = online_clients[idx];
    //         break;
    //     }
    // }
    pthread_mutex_unlock(&mutex_db);

    /* 发送私聊消息给接收者 */
    // if (receiverSocket != -1) {
    //     send(receiverSocket, directMessage, strlen(directMessage), 0);
    // } else {
    //     /* 如果接收者不在线，给发送者发送提示信息 */
    //     char offlineMsg[BUFFER_SIZE];
    //     snprintf(offlineMsg, sizeof(offlineMsg), "%s is offline or does not exist.", receiverName);
    //     send(sockfd, offlineMsg, strlen(offlineMsg), 0);
    // }
    return 0;
   
}
/* 服务器执行群聊操作 */
int handle_group_chat(int client_sockfd, struct json_object *message)
{
    /* 获取群聊信息 */
    struct json_object *groupChat = message;
    struct json_object *userName = json_object_object_get(groupChat, "userName");
    struct json_object *group_message = json_object_object_get(groupChat, "message");

    /* 将群聊信息转换为字符串 */
    const char *userNameVal = json_object_to_json_string(userNameVal);
    const char *messageVal = json_object_to_json_string(messageVal);

    /* 构建群聊消息 */
    char groupMessage[BUFFER_SIZE];
    snprintf(groupMessage, sizeof(groupMessage), "%s: %s", userName, message);

    /* 发送群聊消息给所有在线客户端 */
    pthread_mutex_lock(&mutex_db);
    // for (int idx = 0; idx < online_clients.size(); idx++) {
    //     send(online_clients[idx], groupMessage, strlen(groupMessage), 0);
    // }
    pthread_mutex_unlock(&mutex_db);

    return 0;
}
#endif
