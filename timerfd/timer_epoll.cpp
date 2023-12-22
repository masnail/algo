#include <string.h>
#include "timer_channel.h"
#include "timer.h"
#include "timer_epoll.h"

const int kSize = 1024;

EpollTimer::EpollTimer()
    :epoll_fd_(epoll_create(1))
    ,events_(new epoll_event[kSize])
{
    if(epoll_fd_ == -1) {
        LOG(ERROR) << "epoll_create failed";
    }
    memset(events_,0, sizeof(kSize*sizeof(epoll_event)));
}
EpollTimer::~EpollTimer()
{
    if (epoll_fd_ != -1) {
        epoll_fd_ = -1;
    }
    delete[] events_;
}

void EpollTimer::UpdateChannel(ChannelTimer* ch)
{    
    int fd = ch->Fd();
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = ch;    //关键是这里
    ev.events = ch->Event();    //从ch中获取需要监听的事件

    if (ch->IsInEpoll()) {  //表示已在epoll上
		//新添加的 判断是否是不监听任何事件
        if (ch->IsNoneEvent()) {    //表明该channel已不监听任何事件，就可以进行EPOLL_CTL_DEL
            DelChannel(ch);
        }
        else {
            int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev);
            if (ret == -1) {
                LOG(ERROR) << "epoll_ctl mod failed";
            }
        }
    }
    else {
        int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev);
        if (ret == -1) {
            LOG(ERROR) << "epoll_ctl add failed";
        }
        ch->SetInEpoll(true);
    }
}

void EpollTimer::DelChannel(ChannelTimer* ch)
{
    //添加了这个判断，先判断该channel是否已被epoll监听，是已被监听才进行删除
    if (ch->IsInEpoll()) {
        int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, ch->Fd(), nullptr);
        if (ret == -1) {
            LOG(ERROR) << "epoll_ctl DEL failed";
        }
        ch->SetInEpoll(false);
    }
}


void EpollTimer::EpollWaitActive(std::vector<ChannelTimer*>& active, int timeout)
{
    int nums = epoll_wait(epoll_fd_, events_, kSize, timeout);
    if (nums == -1) {
        LOG(ERROR) << "epoll_wait failed, fd:" << epoll_fd_;
    }
    for (int i = 0; i < nums; ++i) {
        ChannelTimer* ch = static_cast<ChannelTimer*>(events_[i].data.ptr);
        ch->SetRevents(events_[i].events);    //设置从epoll中返回的事件
        active.emplace_back(ch);
    }
}