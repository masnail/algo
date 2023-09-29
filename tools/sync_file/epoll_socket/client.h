#pragma once

#ifndef ___CLIENT_SOCKET_HEADER_H___
#define ___CLIENT_SOCKET_HEADER_H___

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <random>
#include <string>
#include <cmath>

#include "file_operation/sync_file.h"
#include "epoll_socket/epoll_socket.h"

/*
 *
 *  此为client的基类，继承EpollSocket，实现send和recv作为私有函数
 *
 *  主要功能是端口和IP链接Server服务器并实现特定功能
 *
 *
 */

using namespace std;

class ClientImpl : private EpollSocket
{
private:
    /* data */
    int server_port;
    string server_ip;
    int client_id;

    int socket_fd;
    struct sockaddr_in serv_adr;
public:
    ClientImpl(int server_port, string server_ip);
    ~ClientImpl();

public:
    /**
     * @brief 根据构造函数的IP和端口，初始化Client
     * 
     * @return true         初始化成功
     * @return false        初始化失败
     */
    bool InitClient();

    /**
     * @brief 对外提供socket函数，方便发送和接受数据
     * 
     * @return int      socket fd
     */
    int getSocketFD();

};


#endif