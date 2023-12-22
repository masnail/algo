#pragma once

#include <sys/epoll.h>
#include <vector>

class ChannelTimer;

class EpollTimer
{
public:
    EpollTimer();
    ~EpollTimer();
    void UpdateChannel(ChannelTimer* ch);
    void DelChannel(ChannelTimer* ch);
    int GetEpollFD()const { return epoll_fd_; }
    void EpollWaitActive(std::vector<ChannelTimer*>& active, int timeout = 10);
private:
    int epoll_fd_;
    struct epoll_event* events_;
};