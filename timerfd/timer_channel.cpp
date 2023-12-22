#include "timer_channel.h"

ChannelTimer::ChannelTimer(std::shared_ptr<EpollTimer> ep_ptr, int fd)
    : fd_(fd)
    , epoll_ptr_(ep_ptr)
    , events_(0)
    , revents_(0)
    , isInEpoll_(false)
{
}

void ChannelTimer::SetEvents(int events)
{
    events_ = events;
}
int ChannelTimer::Event()const
{
    return events_;
}
void ChannelTimer::SetRevents(int revents)
{
    revents_ = revents;
}
int ChannelTimer::Revent()const
{
    return revents_;
}

bool ChannelTimer::IsInEpoll()
{
    return isInEpoll_ == true;
}

void ChannelTimer::SetInEpoll(bool in)
{
    isInEpoll_ = in;
}

int ChannelTimer::Fd()const
{
    return fd_;
}


void ChannelTimer::HandleEvent()
{
    HandleEventWithGuard();
}

void ChannelTimer::Remove()
{
    epoll_ptr_->DelChannel(this);
}

void ChannelTimer::Update()
{
    epoll_ptr_->UpdateChannel(this);
}

void ChannelTimer::HandleEventWithGuard()
{
    //当事件为挂起并没有可读事件时
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (closeCallback_) {
            printf("channel closeCallback\n");
            closeCallback_();
        }
    }
    if (revents_ & EPOLLERR) {
        if (errorCallback_)
            errorCallback_();
    }
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {  //关于读的事件
        if (readCallback_)
            readCallback_();
    }
    if (revents_ & EPOLLOUT) {   //关于写的事件
        if (writeCallback_)
            writeCallback_();
    }
}

void ChannelTimer::GetActiveChannel(std::vector<ChannelTimer*>& active) {
    active.clear();
    epoll_ptr_->EpollWaitActive(active,10000);
}
