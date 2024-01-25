#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "scFunc.h"


#define SERVER_PORT 6666
#define MAX_LISTEN 128
#define SERVER_IPADDRESS "172.31.173.216"
#define BUF_SIZE 128


int main()
{
    /* 创建客户端套接字 */
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("client_socket error");
    }

    /* 存储的要连接的服务器的 */
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    /* 绑定 */
    serverAddress.sin_family = AF_INET; /* 协议族 */
    serverAddress.sin_port = htons(SERVER_PORT);/* 端口 大端 短整型 */
    int ret = inet_pton(AF_INET, SERVER_IPADDRESS, (void *)&serverAddress.sin_addr.s_addr);
    if (ret != 1)
    {
        perror("inet_pton error");
    }
    /* 连接 */
    ret = connect(client_socket,(struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (ret == -1)
    {
        perror("conner error");
        exit(-1);
    }
    /* 连接成功之后开始进行操作 */
    int choice = 0;
    struct message msg;
    
    while (1)
    {
        /* 打印菜单 */
        print_menu();
        /* 功能选择 */
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {   
        /* 选择登录 */
        case Login:
            client_login(&msg);
            break;
        /* 选择注册 */
        case Register:
            client_register(&msg);
            break;
        /* 选择私聊 */
        case Direct_chat:
            client_direct_chat(&msg);
            break;
        /* 选择群聊 */
        case Groups_chat:
            client_group_chat(&msg);
            break;
        /* 选择无效 */
        default:
            printf("please choose right function");
            break;
        }
        
    }
    
    
    close(client_socket);
  
    return 0;
}