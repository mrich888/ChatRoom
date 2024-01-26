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
/* 服务器执行登录操作 */
int handle_login(int sockfd, struct json_object *message)
{
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


/* 登录之后到的回复 */
int  login_callback (void *arg)
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

