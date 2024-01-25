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
#include "client_handle.h"


#define SERVER_PORT 8080
#define SERVER_IP   "172.31.173.216"
#define BUFFER_SIZE 128

int main()
{
    /* 客户端套接字 */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
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
    ret = connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (ret == -1)
    {
        perror("connect error");
        exit(-1);
    }

    /* 连接成功后就可以执行任务了 */ 
    int choice;
    struct json_object * user = json_object_new_object();

    while (1)
    {
        /* 首先开始选择功能 */
        print_menu();
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case Login:
            client_login(sockfd, user);
            break;
        case Register:
            client_register(sockfd, user);
            break;
        case Direct_chat:
            client_direct_chat();
            break;
        case Groups_chat:
            client_group_chat();
        default:
            printf("Please select the correct function!");
            print_menu();
            break;
        }

    }
    /* 休息5S */
    sleep(5);

    /* 关闭文件描述符 */
    close(sockfd);

    return 0;
}