#pragma once

#ifndef ___SYNC_FILE_HEAD_H____
#define ___SYNC_FILE_HEAD_H____

#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "epoll_socket/epoll_socket.h"

class SyncFileTool : private EpollSocket
{
private:
    char file_name_[256];
    std::fstream file_os_;

public:
    SyncFileTool();
    ~SyncFileTool();

private:
    /**
     * @brief 使用读取二进制的方式打开文件
     * 
     * @param file_name         文件名
     * @param len               文件名长度
     * @return true
     * @return false 
     */
    bool InitRead(char file_name[], int64_t len);

    /**
     * @brief 使用写入二进制的方式打开文件，如果存在会被覆盖，不存在就创建
     * 
     * @param file_name         文件名
     * @param len               文件名长度
     * @return true 
     * @return false 
     */
    bool InitWrite(char file_name[], int64_t len);

    /**
     * @brief 获取文件的长度，单位字节
     * 
     * @return int64_t          返回文件长度
     */
    int64_t GetFileSize();

    /**
     * @brief 按照指定长度进行文件读取，最后不足指定长度会被修改
     * 
     * @param buff              存放读取文件后的内容
     * @param len               读取文件的长度，读取不足原先的长度会被修改
     * @return true 
     * @return false 
     */
    bool ReadFile(char buff[], int64_t* len);

    /**
     * @brief 按照指定长度进行文件写入
     * 
     * @param buff              存放写入文件的数据
     * @param len               写入数据的长度
     * @return true 
     * @return false 
     */
    bool WriteFile(char buff[], int64_t* len);

public:
    /**
     * @brief 单线程整个文件，通过socket接受文件并保存到磁盘
     * 
     * @param socket_fd 
     * @param filename 
     * @param len 
     * @return true 
     * @return false 
     */
    bool SyncRecvFile(int socket_fd, char filename[], int64_t len);

    /**
     * @brief 单线程整个文件，通过socket发送文件
     * 
     * @param socket_fd 
     * @param filename 
     * @param len 
     * @return true 
     * @return false 
     */
    bool SyncSendFile(int socket_fd, char filename[], int64_t len);
};

#endif