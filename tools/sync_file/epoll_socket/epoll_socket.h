#pragma once

#ifndef ___EPOLL_SOCKET_HEADER_H___
#define ___EPOLL_SOCKET_HEADER_H___

#include <iostream>
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


/*
 *
 *  此为Socket发送和接受的优化，方便Client和Server服用，进行大数据的发送
 *
 *  主要是对send和recv进行提取，方便组合发送
 *
 *
 */

using namespace std;

class EpollSocket
{
private:
    /* data */
public:
    EpollSocket(/* args */);
    ~EpollSocket();

public:
    /**
     * @brief 优化recv socket的数据接受，保证接受数据长度一定为dataSize
     * 
     * @param socket_fd     socket fd
     * @param buf           接受数据空间
     * @param dataSize      需要接受数据的长度
     * @return int          是否接受数据成功，0：一次性接受完毕，1：多次接受完毕：-1：接受失败
     */
    int RecvSocket(int socket_fd, char buf[], int64_t dataSize);

    /**
     * @brief 优化send socket的数据接受，保证发送数据长度一定为dataSize
     * 
     * @param socket_fd     socket fd
     * @param buf           发送数据空间
     * @param dataSize      需要发送数据的长度
     * @return int          是否发送数据成功，0：一次性发送完毕，1：多次发送完毕：-1：接受失败
     */
    int SendSocket(int socket_fd, char buf[], int64_t dataSize);

    /**
     * @brief 对socket recv数据的封装，分为两次，第一次为接受数据的长度，第二次开始接受数据
     * 
     * @param socket_fd     socket fd
     * @param buff          接受数据空间
     * @return int64_t      接受数据的长度
     */
    int64_t RecvBuff(int socket_fd, char buff[]);

    /**
     * @brief 对socket send数据的封装，分为两次，第一次为发送数据的长度，第二次开始发送数据
     * 
     * @param socket_fd     socket fd
     * @param buff          发送数据空间
     * @param len           发送数据的长度
     * @return true         发送数据成功
     * @return false        发送数据失败
     */
    bool SendBuff(int socket_fd, char buff[], int64_t len);
};

#endif