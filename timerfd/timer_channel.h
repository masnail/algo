#pragma once

#include <iostream>
#include <functional>
#include <unistd.h>
#include <fcntl.h>
#include <memory>

#include "timer_epoll.h"



class ChannelTimer
{
public:
    using EventCallback = std::function<void()>;

public:
    ChannelTimer(std::shared_ptr<EpollTimer> ep_ptr, int fd);
    ~ChannelTimer() = default;
    void SetEvents(int events);
    int Event() const;
    void SetRevents(int events);
    int Revent() const;

    bool IsInEpoll();
    void SetInEpoll(bool in);
    int Fd() const;

    void SetReadCallback(EventCallback cb) { readCallback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

    void EnableReading() { events_ |= (EPOLLIN | EPOLLPRI); Update(); }    //注册可读事件
    void DisableReading() { events_ &= ~(EPOLLIN | EPOLLPRI); Update(); }    //注销可读事件
    void EnableWriting() { events_ |= EPOLLOUT; Update(); }            //注册可写事件
    void DisableWriting() { events_ &= ~EPOLLOUT; Update(); }        //注销可写事件
    void DisableAll() { events_ = 0; Update(); }    //注销所有事件

    //返回fd当前的事件状态
    bool IsNoneEvent() const { return events_ == 0; }
    bool IsWrite() const { return events_ & EPOLLOUT; }
    bool IsRead() const { return events_& (EPOLLIN | EPOLLPRI); }

    void HandleEvent();
    void Remove();

    void GetActiveChannel(std::vector<ChannelTimer*>& active);
private:
    void Update();
    void HandleEventWithGuard();
private:

    int fd_;
    int events_;
    int revents_;
    bool isInEpoll_;

    std::shared_ptr<EpollTimer> epoll_ptr_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};