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
#include <signal.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <cmath>

#include "epoll_socket/threadpool.h"
#include "file_operation/sync_file.h"
#include "epoll_socket/epoll_socket.h"

/*
 *
 *  此为server的基类，继承EpollSocket，实现send和recv作为私有函数
 *
 *  主要功能是实现端口和IP绑定，配合epoll实现Server功能
 *
 *
 */

using namespace std;

#define EPOLL_SIZE 50

class ServerImpl : private EpollSocket
{
private:
    /* data */
    string server_ip;
    int server_port;

    // socket
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr;
    struct sockaddr_in clnt_adr;
    socklen_t adr_sz;

    // epoll
    struct epoll_event *ep_events;
    struct epoll_event event;
    int epfd;
    int event_cnt;

    //线程池
    ThreadPool pool;

public:
    ServerImpl(int server_port, string server_ip);
    ~ServerImpl();

public:
    /**
     * @brief 根据构造函数的IP和端口，初始化Server
     * 
     * @param pool_size         线程池中线程的个数，默认为4
     * @return true             成功
     * @return false            失败
     */
    bool InitServer(int pool_size = 4);

private:
    /**
     * @brief 使用epoll网络编程
     * 
     */
    void ExecEpoll();
};

#endif