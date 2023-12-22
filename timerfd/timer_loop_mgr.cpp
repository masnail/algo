#include "timer_loop_mgr.h"
#include "timer_channel.h"
#include "timer_queue.h"
#include <signal.h>

#include <thread>
#include <iostream>

// 创建wakeupfd，用来notify唤醒处理回调任务操作
int CreateEventFD()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        printf("eventfd error:%d\n", errno);
    }
    return evtfd;
}

TimerEventMgr::TimerEventMgr()
    :threadId_(std::this_thread::get_id())
    ,quit_(false)
    , ep_ptr_(std::make_shared<EpollTimer>())
    ,wakeupFd_(CreateEventFD())
    ,wakeupChannel_(std::make_unique<ChannelTimer>(ep_ptr_,wakeupFd_))
    ,timer_queue_(std::make_unique<TimerQueue>(ep_ptr_))
{
    // 设置wakeupfd的发生事件后的回调操作
    wakeupChannel_->SetReadCallback([this]() {HandleRead(); });
    //监听wakeupchannel_的EPOLLIN读事件了
    wakeupChannel_->EnableReading();

    //  RAII  初始化就开线程，定时器循环
    std::shared_ptr<std::thread> loop_thread = std::make_shared<std::thread>([this](){
        TimerLoop();
    });
    loop_thread->detach();
}

void TimerEventMgr::Destory()
{
    wakeupChannel_->DisableAll();
    wakeupChannel_->Remove();
    ::close(wakeupFd_);
}

void TimerEventMgr::TimerLoop()
{
    quit_ = false;
    while (!quit_) {
        wakeupChannel_->GetActiveChannel(activeChannels_);
        for (auto& active : activeChannels_) {
            active->HandleEvent();
        }
    }
}

void TimerEventMgr::QuitLoop() {
    quit_ = true;
}

void TimerEventMgr::HandleRead()
{
    uint64_t one = 1;
    auto n = ::read(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)){
        printf("TimerEventMgr::HandleRead() reads %lu bytes \n", n);
    }
}

//在给定的时间执行定时器
int64_t TimerEventMgr::AddtimerAtTime(TimerStamp time, TimerCallback cb)
{
    return timer_queue_->AddTimer(std::move(cb), time, 0.0);
}

//在给定的时间间隔后执行定时器
int64_t TimerEventMgr::AddtimerAfterTime(double delay_seconds, TimerCallback cb)
{
    TimerStamp time(AddTime(TimerStamp::NowTime(), delay_seconds));
    return AddtimerAtTime(time, std::move(cb));
}

//每个一个时间间隔就执行一次定时器
int64_t TimerEventMgr::AddtimerEveryTime(double interval_seconds, TimerCallback cb)
{
    TimerStamp time(AddTime(TimerStamp::NowTime(), interval_seconds));
    return timer_queue_->AddTimer(std::move(cb), time, interval_seconds);
}

void TimerEventMgr::StopTimer(int64_t timerId)
{
    return timer_queue_->StopTimer(timerId);
}

